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
static void _SoAd_HandleSoConStateInvalid(SoAd_SoConIdType SoConId);
static void _SoAd_HandleSoConStateNew(SoAd_SoConIdType SoConId);
static void _SoAd_HandleSoConStateBind(SoAd_SoConIdType SoConId);

static void _SoAd_SocketRxRoutine(SoAd_SoConIdType *SoConId);
static void _SoAd_SocketConnectRoutine(SoAd_SoConIdType *SoConId);
static void _SoAd_SocketListenRoutine(SoAdConGroupHandler_t *SoConGr);

static Std_ReturnType _SoAd_CreateSocket(SoAd_SoConIdType SoConId);
static Std_ReturnType _SoAd_BindSocket(SoAd_SoConIdType SoConId);

static Std_ReturnType _SoAd_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon);

static Std_ReturnType _SoAd_IpCmp(char *s1, char *s2, uint32 size);
/* ***************************** [ DATAS     ] ****************************** */

//TODO: SoAd Configuration
SoAd_UpperFncTable_t _SoAd_UpperFunctionTable[] =
{
    {
        &DoIP_SoAdIfRxIndication, //UpperIfRxIndication
        NULL_PTR, //UpperIfTriggerTransmit
        NULL_PTR, //UpperIfTxConfirmation
        &DoIP_SoAdTpStartOfReception, //UpperTpStartOfReception
        NULL_PTR, //UpperTpCopyRxData
        NULL_PTR, //UpperTpRxIndication
        NULL_PTR, //UpperTpCopyTxData
        NULL_PTR, //UpperTpTpTxConfirmation
        &DoIP_SoConModeChg, //UpperSoConModeChg
        NULL_PTR //LocalIpAddrAssignmentChg
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
  retGr->W32SockListen = INVALID_SOCKET;
  retGr->W32SockListenState = _SOAD_SOCK_STATE_INVALID;
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
  soAdSock->W32SockState = _SOAD_SOCK_STATE_INVALID;
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

/*
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
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_NEW;
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
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_BIND;
    }
  }

  if(ret != E_NOT_OK)
  {
    DWORD threadId;
    HANDLE threadHdl = CreateThread( NULL, 1024,
              ( LPTHREAD_START_ROUTINE ) _SoAd_SocketRxRoutine,
              &_SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
              &threadId );

    if(threadHdl)
    {
      _SoAd_DynSoConArr[SoConId].W32Sock = newSock;
      _SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
      _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_CONNECTED;

      //TODO: state = SOAD_SOCON_RECONNECT for UDP listen only
      _SOAD_GET_SOCON_FNCTBL(SoConId).UpperSoConModeChg(SoConId, SOAD_SOCON_ONLINE);

      ResumeThread(threadHdl);
    }else
    {
      WSACleanup();
      ret = E_NOT_OK;

      _SoAd_DynSoConArr[SoConId].W32Sock = INVALID_SOCKET;
      _SoAd_DynSoConArr[SoConId].W32Thread.Id = -1;
      _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = NULL_PTR;
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_INVALID;
    }
  }

  return ret;
}
*/

void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId)
{
  //handle open - close desired
  switch(_SoAd_DynSoConArr[(SoConId)].W32SockState)
  {
    case _SOAD_SOCK_STATE_INVALID:
      _SoAd_HandleSoConStateInvalid(SoConId);
      break;
    case _SOAD_SOCK_STATE_NEW:
      _SoAd_HandleSoConStateNew(SoConId);
      break;
    case _SOAD_SOCK_STATE_BIND:
      _SoAd_HandleSoConStateBind(SoConId);
      break;
    default:
      break;
  }
}

static void _SoAd_HandleSoConStateInvalid(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;

  if(_SOAD_CHECK_SOCCON_NEED_OPEN(SoConId))
  {
    ret = _SoAd_CreateSocket(SoConId);

    if(ret == E_OK)
    {
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_NEW;
      _SOAD_CLEAR_SOCON_REQMASK(SoConId, _SOAD_SOCCON_REQMASK_OPEN);
    }else
    {
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_INVALID;
    }
  }
}

static void _SoAd_HandleSoConStateNew(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;

  ret = _SoAd_BindSocket(SoConId);

  if(ret == E_OK)
  {
    _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_BIND;
  }else
  {
    _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_INVALID;
  }
}

static void _SoAd_HandleSoConStateBind(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;
  DWORD threadId;
  HANDLE threadHdl;

  if(_SOAD_IS_TCP_SERVER_SOCON(SoConId))
  {
    if(_SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState == _SOAD_SOCK_STATE_BIND)
    {
      //TCP server
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) _SoAd_SocketListenRoutine,
                _SOAD_GET_SOCON_GROUP(SoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl)
      {
        _SOAD_GET_SOCON_FNCTBL(SoConId).UpperSoConModeChg(SoConId, SOAD_SOCON_RECONNECT);
        _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_LISTENING;
        _SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState = _SOAD_SOCK_STATE_LISTENING;
        ResumeThread(threadHdl);
      }else
      {
        ret = E_NOT_OK;
      }
    }else
    {
      //A thread listening has been created for this group
      _SOAD_GET_SOCON_FNCTBL(SoConId).UpperSoConModeChg(SoConId, SOAD_SOCON_RECONNECT);
      _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_LISTENING;
      _SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState = _SOAD_SOCK_STATE_LISTENING;
    }
  }else
  {
    if(_SOAD_IS_UDP_SOCON(SoConId))
    {
      //create thread for receive UDP data
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) _SoAd_SocketRxRoutine,
                &_SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl)
      {
        _SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
        _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;
        //TODO: state = SOAD_SOCON_RECONNECT for UDP listen only
        _SOAD_GET_SOCON_FNCTBL(SoConId).UpperSoConModeChg(SoConId, SOAD_SOCON_ONLINE);
        _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_CONNECTED;
        ResumeThread(threadHdl);
      }else
      {
        ret = E_NOT_OK;
      }

    }else
    {
      //TCP client, create thread for Connect
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) _SoAd_SocketConnectRoutine,
                &_SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl)
      {
        _SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
        _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;
        _SOAD_GET_SOCON_FNCTBL(SoConId).UpperSoConModeChg(SoConId, SOAD_SOCON_RECONNECT);
        _SoAd_DynSoConArr[SoConId].W32SockState = _SOAD_SOCK_STATE_CONNECTING;
        ResumeThread(threadHdl);
      }else
      {
        ret = E_NOT_OK;
      }
    }//end check _SOAD_IS_UDP_SOCON

    //cleanup resource for case UDP or TCP client
    if(ret == E_NOT_OK)
    {
      WSACleanup();
      _SoAd_DynSoConArr[SoConId].W32Sock = INVALID_SOCKET;
      _SoAd_DynSoConArr[SoConId].W32Thread.Id = -1;
      _SoAd_DynSoConArr[SoConId].W32Thread.Hdl = NULL_PTR;
    }
  }//end check _SOAD_IS_TCP_SERVER_SOCON
}

static Std_ReturnType _SoAd_CreateSocket(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  SoAdConGroupHandler_t *SoConGr = _SOAD_GET_SOCON_GROUP(SoConId);
  int iResult;
  WSADATA wsaData;

  SOCKET newSock = INVALID_SOCKET;

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
      if(_SOAD_IS_TCP_SERVER_SOCON(SoConId))
      {
        //sever group
        if(_SOAD_GET_SOCON_GROUP(SoConId)->W32SockListen == INVALID_SOCKET)
        {
          _SOAD_GET_SOCON_GROUP(SoConId)->W32SockListen = newSock;
          _SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState = _SOAD_SOCK_STATE_NEW;
        }

      }else
      {
        //TCP client or UDP
        _SoAd_DynSoConArr[SoConId].W32Sock = newSock;
      }
    }
  }

  return ret;
}

static Std_ReturnType _SoAd_BindSocket(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  SoAdConGroupHandler_t *SoConGr = _SOAD_GET_SOCON_GROUP(SoConId);
  SoAdSock_t *thisSoCon = &_SoAd_DynSoConArr[SoConId];

  int iResult;
  WSADATA wsaData;

  struct sockaddr_in sockaddr;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != NO_ERROR) {
      ret = E_NOT_OK;
  }

  if(ret != E_NOT_OK)
  {
    // Bind the socket.
    sockaddr.sin_family = SoConGr->AfType;
    sockaddr.sin_addr.s_addr = inet_addr(SoConGr->LocalAddress);
    sockaddr.sin_port = htons(SoConGr->LocalPort);

    if(_SOAD_IS_TCP_SERVER_SOCON(SoConId))
    {
      //sever group
      if(_SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState == _SOAD_SOCK_STATE_NEW)
      {
        iResult = bind(_SOAD_GET_SOCON_GROUP(SoConId)->W32SockListen,
            (SOCKADDR *) &sockaddr, sizeof (sockaddr));
        if (iResult == SOCKET_ERROR) {
            closesocket(thisSoCon->W32Sock);
            WSACleanup();
            ret = E_NOT_OK;
        }else
        {
          _SOAD_GET_SOCON_GROUP(SoConId)->W32SockListenState = _SOAD_SOCK_STATE_BIND;
        }
      }
    }else
    {
      //TCP client or UDP
      iResult = bind(thisSoCon->W32Sock, (SOCKADDR *) &sockaddr, sizeof (sockaddr));
      if (iResult == SOCKET_ERROR) {
          closesocket(thisSoCon->W32Sock);
          WSACleanup();
          ret = E_NOT_OK;
      }
    }
  }

  return ret;
}

void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId)
{
  BufReq_ReturnType upperBufReqRet = BUFREQ_E_NOT_OK;
  PduLengthType upperBufferSizePtr;
  PduInfoType pduInfo;

  if(_SoAd_DynSoConArr[SoConId].RxLength > 0)
  {
    if(_SOAD_IS_TCP_SOCON(SoConId))
    {
      pduInfo.SduLength = _SoAd_DynSoConArr[SoConId].RxLength;
      pduInfo.SduDataPtr = _SoAd_DynSoConArr[SoConId].RxBuff;

      upperBufReqRet = _SOAD_GET_SOCON_FNCTBL(SoConId).UpperTpStartOfReception(
          _SoAd_DynSoConArr[SoConId].UpperRxPduId,
          &pduInfo, pduInfo.SduLength, &upperBufferSizePtr);

      _SoAd_DynSoConArr[SoConId].RxLength = 0;
    }
  }
}

static void _SoAd_SocketRxRoutine(SoAd_SoConIdType *SoConId)
{
  int resultLength = -1;

  SoAd_SoConIdType thisSoConId = *SoConId;
  SoAdSock_t *thisSoAdSock = &_SoAd_DynSoConArr[thisSoConId];

  PduInfoType pduInfo;
  uint8 rxBuffer[SOAD_CFG_SOCON_RX_BUFF_SIZE];

  while(1)
  {
    resultLength = recv(thisSoAdSock->W32Sock, (char *)&rxBuffer[0], SOAD_CFG_SOCON_RX_BUFF_SIZE, 0);

    if(resultLength != -1)
    {
      if(_SOAD_IS_UDP_SOCON(thisSoConId))
      {
        pduInfo.SduLength = resultLength;
        pduInfo.SduDataPtr = &rxBuffer[0];

        _SOAD_GET_SOCON_FNCTBL(thisSoConId).UpperIfRxIndication(
            _SoAd_DynSoConArr[thisSoConId].UpperRxPduId,  &pduInfo);

        thisSoAdSock->RxLength = 0;

      }else if(_SOAD_IS_TCP_SOCON(thisSoConId))
      {
        if(resultLength != 0)
        {
          thisSoAdSock->RxLength = resultLength;

          memset(thisSoAdSock->RxBuff, 0, SOAD_CFG_SOCON_RX_BUFF_SIZE);
          memcpy(thisSoAdSock->RxBuff, &rxBuffer[0], resultLength);
        }else
        {
          //connection lost
          _SOAD_GET_SOCON_FNCTBL(thisSoConId).UpperSoConModeChg(thisSoConId, SOAD_SOCON_RECONNECT);

          thisSoAdSock->W32SockState = _SOAD_SOCK_STATE_LISTENING;

          closesocket(thisSoAdSock->W32Sock);
          TerminateThread(thisSoAdSock->W32Thread.Hdl, 0);
        }
      }else
      {
        //Unknown
      }
    }else
    {
      //Exceed SOAD_CFG_SOCON_RX_BUFF_SIZE, TODO: handle ?
    }
  }
}

void _SoAd_SocketListenRoutine(SoAdConGroupHandler_t *SoConGr)
{
  int iResult;
  WSADATA wsaData;

  SOCKADDR_IN addr;
  int addrlen = sizeof(addr);

  DWORD threadId;
  HANDLE threadHdl;

  Std_ReturnType ret = E_NOT_OK;

  SoAd_SoConIdType acceptedSoConId;
  SoAdSock_t *thisSoAdSock;

  SOCKET listenSocket = SoConGr->W32SockListen;
  SOCKET acceptSocket = INVALID_SOCKET;

  // Initialize WIN32SOCK
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != NO_ERROR) {
      return;
  }

  while(1)
  {
    //LISTEN
    iResult = listen(listenSocket, 1);

    if (iResult == SOCKET_ERROR)
    {
      closesocket(listenSocket);
      WSACleanup();
      break;
    }

    //ACCEPT
    acceptSocket = accept(listenSocket, (SOCKADDR*)&addr, &addrlen);
    if (acceptSocket == INVALID_SOCKET)
    {
      closesocket(listenSocket);
      WSACleanup();
      break;
    }

    ret = _SoAd_FindMatchSocket(&addr, &acceptedSoConId);
    if(ret == E_OK)
    {

      thisSoAdSock = &_SoAd_DynSoConArr[acceptedSoConId];

      if(thisSoAdSock->W32SockState == _SOAD_SOCK_STATE_LISTENING)
      {
        thisSoAdSock->W32Sock = acceptSocket;

        //create thread for receive client data
        threadHdl = CreateThread( NULL, 1024,
                  ( LPTHREAD_START_ROUTINE ) _SoAd_SocketRxRoutine,
                  &(thisSoAdSock->SoAdSoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl)
        {
          thisSoAdSock->W32Thread.Id = threadId;
          thisSoAdSock->W32Thread.Hdl = threadHdl;

          _SOAD_GET_SOCON_FNCTBL(acceptedSoConId).UpperSoConModeChg(acceptedSoConId, SOAD_SOCON_ONLINE);

          thisSoAdSock->W32SockState = _SOAD_SOCK_STATE_ACCEPTED;
          ResumeThread(threadHdl);
        }else
        {
          break;
          //TODO: handle ?
        }
      }
    }
  }
}

Std_ReturnType _SoAd_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon)
{
  SoAd_SoConIdType soconIdx = 0;
  Std_ReturnType ret = E_NOT_OK;

  char *ip = inet_ntoa(addr->sin_addr);
  uint32 port =  ntohs(addr->sin_port);

  for(soconIdx = 0; soconIdx < _SoAd_DynSoConArrCtn; soconIdx++)
  {
    if((E_OK == _SoAd_IpCmp(&(_SoAd_DynSoConArr[soconIdx].RemoteAddress[0]), ip, SOAD_IPV4_ADD_SIZE)) &&
        _SoAd_DynSoConArr[soconIdx].RemotePort == port)
    {
      *retSocon = soconIdx;
      ret = E_OK;
      break;
    }
  }

  return ret;
}

void _SoAd_SocketConnectRoutine(SoAd_SoConIdType *SoConId)
{
  int resultLength = -1;

  SoAd_SoConIdType curSoConId = *SoConId;
  SoAdSock_t *curSoAdSock = &_SoAd_DynSoConArr[curSoConId];

  while(1)
  {

  }
}

Std_ReturnType _SoAd_IpCmp(char *s1, char *s2, uint32 size)
{
  Std_ReturnType ret = E_OK;
  uint32 idx;

  for(idx = 0; idx < size; idx++)
  {
    if(s1[idx] == '\0')
    {
      break;
    }

    if(s1[idx] != s2[idx])
    {
      ret = E_NOT_OK;
      break;
    }
  }

}
/* ***************************** [ FUNCTIONS ] ****************************** */
