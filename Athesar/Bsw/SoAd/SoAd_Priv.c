/*
 * SoAd_Priv.c
 *
 *  Created on: Jul 30, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd.h"
#include "SoAd_Priv.h"
#include "DoIP_Cbk.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */
SoAd_UpperFncTable_t _SoAd_UpperFunctionTable[] =
{
    {
        &DoIP_SoAdIfRxIndication,
    }
};

SoAdSock_t _SoAd_DynSoConArr[SOAD_CFG_NUM_SOAD_SOCKS];
/* ***************************** [ LOCALS    ] ****************************** */

SoAdConGroupHandler_t *SoAd_CreateSoConGr(SoAd_SoConGrPar_t *SoConGrPar)
{
  SoAdConGroupHandler_t *retGr = NULL;

    //create SoCon Gr
  retGr = (SoAdConGroupHandler_t *)malloc(sizeof(SoAdConGroupHandler_t));

  retGr->AfType = SoConGrPar->AfType;
  retGr->IsServer = SoConGrPar->IsServer;
  memcpy(retGr->LocalAddress, SoConGrPar->LocalAddress, SOAD_IPV4_ADD_SIZE);
  retGr->LocalPort = SoConGrPar->LocalPort;
  retGr->ProtocolType = SoConGrPar->Protocol;
  retGr->SocketType = SoConGrPar->SocketType;
  retGr->SoAdSocketIpAddrAssignmentChgNotification = SoConGrPar->SoAdSocketIpAddrAssignmentChgNotification;
  retGr->SoAdSocketSoConModeChgNotification = SoConGrPar->SoAdSocketSoConModeChgNotification;

  return retGr;
}

PduIdType SoAd_CreateSoCon(SoAdConGroupHandler_t *AssignedGr,   char  RemoteAddress[], uint32 RemotePort, PduIdType *UpperRxPduId, PduIdType *UpperTxPduId, SoAdUpper_t Upper)
{
  SoAdSock_t *soAdSock;

  soAdSock = &_SoAd_DynSoConArr[_SoAd_DynSoConArrCtn];

  soAdSock->GrAssigned = AssignedGr;
  soAdSock->RxBuff = (char *)malloc(SOAD_CFG_SOCON_RX_BUFF_SIZE);
  soAdSock->TxBuff = (char *)malloc(SOAD_CFG_SOCON_TX_BUFF_SIZE);
  soAdSock->SoAdSoConId = _SoAd_DynSoConArrCtn;
  soAdSock->W32SockState = VTCPIP_SOCK_STATE_INVALID;
  soAdSock->W32Sock = INVALID_SOCKET;
  soAdSock->TxPduId = _SoAd_DynSoConArrCtn + 1;
  memcpy(soAdSock->RemoteAddress, &RemoteAddress[0], SOAD_IPV4_ADD_SIZE);
  soAdSock->RemotePort = RemotePort;
  soAdSock->UpperRxPduId = *UpperRxPduId;
  soAdSock->UpperConfTxPduId = *UpperTxPduId;
  soAdSock->Upper = Upper;
  _SoAd_DynSoConArrCtn++;
  return soAdSock->TxPduId;
}

Std_ReturnType _SoAd_OpenUdpSocket(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  SoAdConGroupHandler_t *SoConGr = NULL;
  int iResult;
  WSADATA wsaData;

  SoConGr = (SoAdConGroupHandler_t *)(_SoAd_DynSoConArr[SoConId].GrAssigned);

  // the listening socket to be created
  SOCKET newSock = INVALID_SOCKET;

  // The socket address to be passed to bind
  struct sockaddr_in service;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != NO_ERROR) {
      ret = E_NOT_OK;
  }

  if(ret != E_NOT_OK)
  {
    newSock = socket(SoConGr->AfType, SoConGr->SocketType, SoConGr->ProtocolType);
    if (newSock == INVALID_SOCKET) {
        WSACleanup();
        ret = E_NOT_OK;
    }else
    {
      _SoAd_DynSoConArr[SoConId].W32SockState = VTCPIP_SOCK_STATE_NEW;
    }
  }

  if(ret != E_NOT_OK)
  {
    // Bind the socket.
    service.sin_family = SoConGr->AfType;
    service.sin_addr.s_addr = inet_addr(SoConGr->LocalAddress);
    service.sin_port = htons(SoConGr->LocalPort);

    iResult = bind(newSock, (SOCKADDR *) &service, sizeof (service));
    if (iResult == SOCKET_ERROR) {
        closesocket(newSock);
        WSACleanup();
        ret = E_NOT_OK;
    }else
    {
      _SoAd_DynSoConArr[SoConId].W32SockState = VTCPIP_SOCK_STATE_BIND;
    }
  }

  if(ret != E_NOT_OK)
  {
    _SoAd_DynSoConArr[SoConId].W32Sock = newSock;

    DWORD threadId;
    HANDLE threadHdl = CreateThread( NULL, 1024,
              ( LPTHREAD_START_ROUTINE ) _SoAd_SocketRoutine,
              &_SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
              &threadId );

    if(threadHdl)
    {
      _SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
      _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;
      _SoAd_DynSoConArr[SoConId].W32SockState = VTCPIP_SOCK_STATE_CONNECTED;

      ResumeThread(threadHdl);
    }
  }

  return ret;
}

void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId)
{
  SoAdConGroupHandler_t *SoConGr = (SoAdConGroupHandler_t *)(_SoAd_DynSoConArr[SoConId].GrAssigned);

  //handle open - close desired
  if(_SOAD_IS_SOCON_NEED_OPEN(SoConId) &&
      _SoAd_DynSoConArr[SoConId].W32SockState == VTCPIP_SOCK_STATE_INVALID)
  {
    if(SoConGr->ProtocolType == VTCPIP_IPPROTO_UDP)
    {
      _SoAd_OpenUdpSocket(SoConId);
      _SOAD_CLEAR_SOCON_REQMASK(SoConId, _SOAD_SOCCON_REQMASK_OPEN);
    }
  }
}

void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId)
{
  if(_SoAd_DynSoConArr[SoConId].RxLength > 0)
  {
    //UDP
    PduInfoType pduInfo;
    pduInfo.SduLength = _SoAd_DynSoConArr[SoConId].RxLength;
    pduInfo.SduDataPtr = (uint8 *)_SoAd_DynSoConArr[SoConId].RxBuff;

    _SoAd_UpperFunctionTable[_SoAd_DynSoConArr[SoConId].Upper].UpperIfRxIndication(
        (_SoAd_DynSoConArr[SoConId].UpperRxPduId), &pduInfo);

    _SoAd_DynSoConArr[SoConId].RxLength = 0;
  }
}

void _SoAd_SocketRoutine(SoAd_SoConIdType *SoConId)
{
  int result = -1;
  SoAd_SoConIdType curSoConId = *SoConId;
  SoAdSock_t *curSoAdSock = &_SoAd_DynSoConArr[curSoConId];

  while(1)
  {
    result = recv(curSoAdSock->W32Sock, curSoAdSock->RxBuff, SOAD_CFG_SOCON_RX_BUFF_SIZE, 0);

    if(result > 0)
    {
      curSoAdSock->RxLength = result;
    }
  }
}
/* ***************************** [ FUNCTIONS ] ****************************** */
