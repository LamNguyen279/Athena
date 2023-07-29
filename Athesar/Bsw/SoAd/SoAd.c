/*
 * SoAd.c
 *
 *  Created on: Jul 28, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd.h"
/* ***************************** [ MACROS    ] ****************************** */

/* ***************************** [ TYPES     ] ****************************** */

/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */
static uint32_t SoAd_DynSoConArrCtn = 0;
SoAdSock_t SoAd_DynSoConArr[SOAD_CFG_NUM_SOAD_SOCKS];
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
void SoAd_Init(const SoAd_ConfigType *ConfigPtr) {

}

SoAdConGroupHandler_t *SoAd_CreateSoConGr(SoAd_SoConGrPar_t *SoConGrPar)
{
  int iResult;
  WSADATA wsaData;

  SoAdConGroupHandler_t *retGr = NULL;

      //create SoCon Gr
    retGr = (SoAdConGroupHandler_t *)malloc(sizeof(SoAdConGroupHandler_t));

    retGr->AfType = SoConGrPar->AfType;
    retGr->IsServer = SoConGrPar->IsServer;
    memcpy(retGr->LocalAddress, SoConGrPar->LocalAddress, 14);
    retGr->LocalPort = SoConGrPar->LocalPort;
    retGr->ProtocolType = SoConGrPar->Protocol;
    retGr->SocketType = SoConGrPar->SocketType;
    retGr->SoAdSocketIpAddrAssignmentChgNotification = SoConGrPar->SoAdSocketIpAddrAssignmentChgNotification;
    retGr->SoAdSocketSoConModeChgNotification = SoConGrPar->SoAdSocketSoConModeChgNotification;

  return retGr;
}

PduIdType SoAd_CreateSoCon(SoAdConGroupHandler_t *AssignedGr,   char  RemoteAddress[], uint32 RemotePort, PduIdType *UpperRxPduId, PduIdType *UpperTxPduId)
{
  SoAdSock_t *soAdSock;

  soAdSock = &SoAd_DynSoConArr[SoAd_DynSoConArrCtn];

  soAdSock->GrAssigned = AssignedGr;
  soAdSock->RxBuff = (char *)malloc(255);
  soAdSock->SoAdSoConId = SoAd_DynSoConArrCtn;
  soAdSock->State = VTCPIP_SOCK_INITIALIZED;
  soAdSock->TcpSock = INVALID_SOCKET;
  soAdSock->TxPduId = SoAd_DynSoConArrCtn + 1;
  memcpy(soAdSock->RemoteAddress, &RemoteAddress[0], 14);
  soAdSock->RemotePort = RemotePort;
  soAdSock->UpperRxPduId = UpperRxPduId;
  soAdSock->UpperRxPduId = UpperTxPduId;

  SoAd_DynSoConArrCtn++;
  return soAdSock->TxPduId;
}

void SoAd_MainFunction(void) {

}

Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType *SoConIdPtr) {
  Std_ReturnType ret = E_NOT_OK;

  for(int idx = 0; idx < SoAd_DynSoConArrCtn; idx++)
  {
    if(SoAd_DynSoConArr[idx].TxPduId == TxPduId)
    {
      *SoConIdPtr = SoAd_DynSoConArr[idx].SoAdSoConId;
      break;
    }
  }

  return ret;
}

Std_ReturnType SoAd_GetLocalAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *LocalAddrPtr,
                                 uint8_t *NetmaskPtr, TcpIp_SockAddrType *DefaultRouterPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_SetRemoteAddr(SoAd_SoConIdType SoConId,
                                  const TcpIp_SockAddrType *RemoteAddrPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_GetRemoteAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *IpAddrPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId) {
  Std_ReturnType ret = E_OK;;

  SoAdConGroupHandler_t *SoConGr = NULL;
  int iResult;
  WSADATA wsaData;

  SoConGr = (SoAdConGroupHandler_t *)(SoAd_DynSoConArr[SoConId].GrAssigned);

    // the listening socket to be created
    SOCKET newSock = INVALID_SOCKET;

    // The socket address to be passed to bind
    struct sockaddr_in service;

    //----------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        ret = E_NOT_OK;
    }
    //----------------------
    // Create a SOCKET for listening for
    // incoming connection requests
    newSock = socket(SoConGr->AfType, SoConGr->SocketType, SoConGr->ProtocolType);
    if (newSock == INVALID_SOCKET) {
        WSACleanup();
        ret = E_NOT_OK;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    service.sin_family = SoConGr->AfType;
    service.sin_addr.s_addr = inet_addr(SoConGr->LocalAddress);
    service.sin_port = htons(SoConGr->LocalPort);

    //----------------------
    // Bind the socket.
    iResult = bind(newSock, (SOCKADDR *) &service, sizeof (service));
    if (iResult == SOCKET_ERROR) {
        closesocket(newSock);
        WSACleanup();
        ret = E_NOT_OK;
    }

  //TCP
  // vTcp create socket then socket(), Bind()
  // client  Socket -> TcpIp_TcpConnect -> connect()
  // server  Socket -> TcpIp_TcpListen -> listen()


  return ret;
}

Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort) {
  Std_ReturnType ret = E_NOT_OK;



  return ret;
}
