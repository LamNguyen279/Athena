/*
 * SoAd_Priv.c
 *
 *  Created on: Jul 30, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd_Ram.h"
#include "SoAd.h"
#include "SoAd_Priv.h"
#include "SoAd_Cfg.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
static void soad_HandleSoConStateInvalid(SoAd_SoConIdType SoConId);
static void soad_HandleSoConStateNew(SoAd_SoConIdType SoConId);
static void soad_HandleSoConStateBind(SoAd_SoConIdType SoConId);

static void soad_SocketRxRoutine(SoAd_SoConIdType *SoConId);
static void soad_SocketConnectRoutine(SoAd_SoConIdType *SoConId);
static void soad_SocketListenRoutine(SoAdSoConGr_t *SoConGr);

static void soad_SoConModeChgAllUppers(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode);
static void soad_IfRxIndicationAllUppers(SoAd_SoConIdType SoConId, PduInfoType *Info);

static Std_ReturnType soad_CreateSocket(SoAd_SoConIdType SoConId);
static Std_ReturnType soad_BindSocket(SoAd_SoConIdType SoConId);
static void soad_FreeSoCon(SoAd_SoConIdType SoConId);

static Std_ReturnType soad_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon);

static Std_ReturnType soad_IpCmp(char *s1, char *s2, uint32 size);

static void soad_EnQueueSoconBuffer(SoAd_SoConIdType SoConId, SoAd_SoConBuffer_t *SoConData);
static SoAd_SoConBuffer_t *soad_GetFirstElementSoconQueue(SoAd_SoConIdType SoConId);
static void soad_RemoveFirstElementSoconQueue(SoAd_SoConIdType SoConId);
/* ***************************** [ DATAS     ] ****************************** */

SoAdSoCon_t SoAd_DynSoConArr[SOAD_CFG_NUM_SOCON];
SoAdSoConGr_t SoAd_DynSoConGrArr[SOAD_CFG_NUM_SOCON_GROUP];
/* ***************************** [ LOCALS    ] ****************************** */


void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId)
{
  uint8 dummy;
  switch(SOAD_DYN_SOCON(SoConId).W32SockState)
  {
    case SOAD_W32SOCK_STATE_INVALID:
      soad_HandleSoConStateInvalid(SoConId);
      break;
    case SOAD_W32SOCK_STATE_NEW:
      soad_HandleSoConStateNew(SoConId);
      break;
    case SOAD_W32SOCK_STATE_BIND:
      soad_HandleSoConStateBind(SoConId);
      break;
    case SOAD_W32SOCK_STATE_CONNECTING:
      dummy = SOAD_DYN_SOCON(SoConId).W32SockState;
      break;
    case SOAD_W32SOCK_STATE_CONNECTED:
      dummy = SOAD_DYN_SOCON(SoConId).W32SockState;
      break;
    case SOAD_W32SOCK_STATE_LISTENING:
      dummy = SOAD_DYN_SOCON(SoConId).W32SockState;
      break;
    case SOAD_W32SOCK_STATE_ACCEPTED:
      dummy = SOAD_DYN_SOCON(SoConId).W32SockState;
      break;
    default:
      break;
  }

  if(SOAD_CHECK_SOCON_NEED_CLOSE(SoConId))
  {
    /* free SoCon connection resource */
    if(SOAD_IS_SOCON_DATA_ONGOING(SoConId))
    {
      /*TODO: handle on going TX/RX data */
    }else
    {
      soad_FreeSoCon(SoConId);
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
    }
  }
}

static void soad_CloseSoCon(SoAd_SoConIdType SoConId)
{
  closesocket(SoAd_DynSoConArr[SoConId].W32Sock);
  WSACleanup();
}

static void soad_FreeSoCon(SoAd_SoConIdType SoConId)
{
  /* UDP */
  if(SOAD_IS_UDP_SOCON(SoConId))
  {

  }

  /* TCP */
  if(SOAD_IS_TCP_SOCON(SoConId))
  {

  }

  soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_OFFLINE);

  TerminateThread(SoAd_DynSoConArr[SoConId].W32Thread.Hdl, 0);

  soad_CloseSoCon(SoConId);

  free(SoAd_DynSoConArr[SoConId].RxBuff);
  free(SoAd_DynSoConArr[SoConId].TxBuff);

  _SoAd_InitSocon(SoConId);
}

void _SoAd_InitSocon(SoAd_SoConIdType SoConId)
{
  SoAdSoCon_t *soCon = &SoAd_DynSoConArr[SoConId];

  memset(soCon->RemoteAddress, 0, SOAD_IPV4_ADD_SIZE);

  soCon->RemotePort = 0;

  soCon->RequestMask = SOAD_SOCCON_REQMASK_NON;

  soCon->RxBuff = malloc(SOAD_CFG_SOCON_RX_BUFF_SIZE);
  soCon->TxBuff = malloc(SOAD_CFG_SOCON_TX_BUFF_SIZE);

  soCon->RxSsCopiedLength = 0;

  soCon->SoAdSoConState = SOAD_SOCON_OFFLINE;

  soCon->W32Sock = SOAD_W32_INVALID_SOCKET;

  soCon->W32SockState = SOAD_W32SOCK_STATE_INVALID;

  soCon->W32Thread.Hdl = INVALID_HANDLE_VALUE;
  soCon->W32Thread.Id = SOAD_INVALID_NUMBER;

  //this stuff is for pass over to the thread.
  soCon->W32SoAdSoConId = SoConId;
}

void _SoAd_InitSoConGroup(uint32 SoConGr)
{
  SoAdSoConGr_t *soConGr = &SoAd_DynSoConGrArr[SoConGr];

  soConGr->W32SockListen = SOAD_W32_INVALID_SOCKET;

  soConGr->W32SockListenState = SOAD_W32SOCK_STATE_INVALID;

  soConGr->W32Thread.Hdl = INVALID_HANDLE_VALUE;

  soConGr->W32Thread.Id = SOAD_INVALID_NUMBER;
}

void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId)
{
  BufReq_ReturnType upperBufReqRet = BUFREQ_E_NOT_OK;
  PduLengthType upperBufferSizePtr;
  PduInfoType pduInfo;

  SoAd_CfgSocketRoute_t *socketRoute = &SOAD_GET_TCP_SOCON_SOCKET_ROUTE(SoConId);

  if(SoAd_DynSoConArr[SoConId].RxSsCopiedLength > 0)
  {
    if(SOAD_IS_TCP_SOCON(SoConId))
    {
      pduInfo.SduLength = SoAd_DynSoConArr[SoConId].RxSsCopiedLength;
      pduInfo.SduDataPtr = SoAd_DynSoConArr[SoConId].RxBuff;

      SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpStartOfReception(
          socketRoute->SoAdRxPduRef, &pduInfo, pduInfo.SduLength, &upperBufferSizePtr);

      SoAd_DynSoConArr[SoConId].RxSsCopiedLength = 0;
    }
  }
}

static void soad_HandleSoConStateInvalid(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;

  if(SOAD_CHECK_SOCON_NEED_OPEN(SoConId))
  {
    ret = soad_CreateSocket(SoConId);

    if(ret == E_OK)
    {
      SOAD_DYN_SOCON(SoConId).W32SockState = SOAD_W32SOCK_STATE_NEW;
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);

    }else
    {
      SOAD_DYN_SOCON(SoConId).W32SockState = SOAD_W32SOCK_STATE_INVALID;
    }
  }
}

static void soad_HandleSoConStateNew(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;

  ret = soad_BindSocket(SoConId);

  if(ret == E_OK)
  {
    SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_BIND;
  }else
  {
    SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_INVALID;
  }
}

static void soad_HandleSoConStateBind(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;
  DWORD threadId;
  HANDLE threadHdl;

  if(SOAD_IS_TCP_SERVER_SOCON(SoConId))
  {
    if(SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState == SOAD_W32SOCK_STATE_BIND)
    {
      //TCP server
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) soad_SocketListenRoutine,
                &SOAD_GET_DYN_SOCON_GROUP(SoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl)
      {
        SOAD_GET_DYN_SOCON_GROUP(SoConId).W32Thread.Hdl = threadHdl;
        SOAD_GET_DYN_SOCON_GROUP(SoConId).W32Thread.Id = threadId;

        soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_RECONNECT);
        SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_LISTENING;
        SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState = SOAD_W32SOCK_STATE_LISTENING;
        ResumeThread(threadHdl);
      }else
      {
        ret = E_NOT_OK;
      }
    }else
    {
      //A thread listening has been created for this group
      soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_RECONNECT);

      SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_LISTENING;
      SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState = SOAD_W32SOCK_STATE_LISTENING;
    }
  }else
  {
    if(SOAD_IS_UDP_SOCON(SoConId))
    {      //create thread for receive UDP data
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                &SoAd_DynSoConArr[SoConId].W32SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl != SOAD_INVALID_NUMBER)
      {
        SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
        SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;

        //TODO: state = SOAD_SOCON_RECONNECT for UDP listen only
        soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_ONLINE);

        SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_CONNECTED;

        ResumeThread(threadHdl);
      }else
      {
        ret = E_NOT_OK;
      }

    }else
    {
      //TCP client, create thread for Connect
      threadHdl = CreateThread( NULL, 1024,
                ( LPTHREAD_START_ROUTINE ) soad_SocketConnectRoutine,
                &SoAd_DynSoConArr[SoConId].W32SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl)
      {
        SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
        SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;
        soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_RECONNECT);
        SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_CONNECTING;
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
      SoAd_DynSoConArr[SoConId].W32Sock = INVALID_SOCKET;
      SoAd_DynSoConArr[SoConId].W32Thread.Id = -1;
      SoAd_DynSoConArr[SoConId].W32Thread.Hdl = NULL_PTR;
    }
  }//end check _SOAD_IS_TCP_SERVER_SOCON
}

static Std_ReturnType soad_CreateSocket(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  SoAd_CfgSoConGrp_t *soConGrCfg = &SOAD_GET_SOCON_GROUP(SoConId);
  SoAdSoConGr_t *soConGr = &SOAD_GET_DYN_SOCON_GROUP(SoConId);

  SOCKET newSock = INVALID_SOCKET;

  SOAD_W32_CHECK_ENV(ret);

  if(ret != E_NOT_OK)
  {
    newSock = socket(soConGrCfg->W32AfType, soConGrCfg->W32SocketType, soConGrCfg->W32ProtocolType);
    if (newSock == INVALID_SOCKET) {
        ret = E_NOT_OK;
    }else
    {
      if(SOAD_IS_TCP_SERVER_SOCON(SoConId))
      {
        //sever group
        if(soConGr->W32SockListen == INVALID_SOCKET)
        {
          soConGr->W32SockListen = newSock;
          soConGr->W32SockListenState = SOAD_W32SOCK_STATE_NEW;
        }

      }else
      {
        //TCP client or UDP
        SOAD_DYN_SOCON(SoConId).W32Sock = newSock;
      }
    }
  }else
  {
    WSACleanup();
  }

  return ret;
}

static Std_ReturnType soad_BindSocket(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  SoAd_CfgSoConGrp_t *soConGrCfg = &SoAd_SoConGrArr[SoConId];
  SoAdSoCon_t *thisSoCon = &SoAd_DynSoConArr[SoConId];

  int iResult;

  struct sockaddr_in sockaddr;

  // Initialize Winsock
  SOAD_W32_CHECK_ENV(ret);

  if(ret != E_NOT_OK)
  {
    // Bind the socket.
    sockaddr.sin_family = soConGrCfg->W32AfType;
    sockaddr.sin_addr.s_addr = inet_addr(soConGrCfg->W32LocalAddress);
    sockaddr.sin_port = htons(soConGrCfg->SoAdSocketLocalPort);

    if(SOAD_IS_TCP_SERVER_SOCON(SoConId))
    {
      //sever group
      if(SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState == SOAD_W32SOCK_STATE_NEW)
      {
        iResult = bind(SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListen,
            (SOCKADDR *) &sockaddr, sizeof (sockaddr));
        if (iResult == SOCKET_ERROR) {
            closesocket(thisSoCon->W32Sock);
            WSACleanup();
            ret = E_NOT_OK;
        }else
        {
          SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState = SOAD_W32SOCK_STATE_BIND;
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

static void soad_SocketRxRoutine(SoAd_SoConIdType *SoConId)
{
  int resultLength = -1;

  SoAd_SoConIdType thisSoConId = *SoConId;
  SoAdSoCon_t *thisSoAdSock = &SoAd_DynSoConArr[thisSoConId];

  PduInfoType pduInfo;
  uint8 rxBuffer[SOAD_CFG_SOCON_RX_BUFF_SIZE];

  SoAd_SoConBuffer_t buffData;

  while(1)
  {
    resultLength = recv(thisSoAdSock->W32Sock, (char *)&rxBuffer[0], SOAD_CFG_SOCON_RX_BUFF_SIZE, 0);

    if(resultLength != -1)
    {
      if(SOAD_IS_UDP_SOCON(thisSoConId))
      {
        pduInfo.SduLength = resultLength;
        pduInfo.SduDataPtr = &rxBuffer[0];

        soad_IfRxIndicationAllUppers(thisSoConId, &pduInfo);

        thisSoAdSock->RxSsCopiedLength = 0;

      }else if(SOAD_IS_TCP_SOCON(thisSoConId))
      {
        if(resultLength != 0)
        {
          //upper is IF
          if(SOAD_GET_TCP_SOCON_SOCKET_ROUTE(thisSoConId).SoAdRxUpperLayerType == SOAD_UPPER_IF)
          {
            soad_IfRxIndicationAllUppers(thisSoConId, &pduInfo);
          }else
          {
            buffData.length = resultLength;
            memcpy(&(buffData.data[0]), &rxBuffer[0], resultLength);

            //TODO: store queue
            soad_EnQueueSoconBuffer(thisSoConId, &buffData);

            //start copy SS
            SoAd_DynSoConArr[thisSoConId].RxSsState = SOAD_SS_START;
          }
        }else
        {
          //TODO: when connection lost detected
          if(SOAD_IS_TCP_SERVER_SOCON(thisSoConId))
          {
            thisSoAdSock->W32SockState = SOAD_W32SOCK_STATE_LISTENING;
            soad_SoConModeChgAllUppers(thisSoConId, SOAD_SOCON_RECONNECT);
          }else
          {
            //case TCP client connection
            thisSoAdSock->W32SockState = SOAD_W32SOCK_STATE_INVALID;
            soad_SoConModeChgAllUppers(thisSoConId, SOAD_SOCON_OFFLINE);
          }

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

static void soad_SocketListenRoutine(SoAdSoConGr_t *SoConGr)
{
  int iResult;
  WSADATA wsaData;

  SOCKADDR_IN addr;
  int addrlen = sizeof(addr);

  DWORD threadId;
  HANDLE threadHdl;

  Std_ReturnType ret = E_NOT_OK;

  SoAd_SoConIdType acceptedSoConId;
  SoAdSoCon_t *thisSoAdSock;

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

    ret = soad_FindMatchSocket(&addr, &acceptedSoConId);
    if(ret == E_OK)
    {

      thisSoAdSock = &SoAd_DynSoConArr[acceptedSoConId];

      if(thisSoAdSock->W32SockState == SOAD_W32SOCK_STATE_LISTENING)
      {
        thisSoAdSock->W32Sock = acceptSocket;

        //create thread for receive client data
        threadHdl = CreateThread( NULL, 1024,
                  ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                  &(thisSoAdSock->W32SoAdSoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl)
        {
          thisSoAdSock->W32Thread.Id = threadId;
          thisSoAdSock->W32Thread.Hdl = threadHdl;

          soad_SoConModeChgAllUppers(acceptedSoConId, SOAD_SOCON_ONLINE);

          thisSoAdSock->W32SockState = SOAD_W32SOCK_STATE_ACCEPTED;
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

static Std_ReturnType soad_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon)
{
  SoAd_SoConIdType soconIdx = 0;
  Std_ReturnType ret = E_NOT_OK;

  char *ip = inet_ntoa(addr->sin_addr);
  uint32 port =  ntohs(addr->sin_port);

  for(soconIdx = 0; soconIdx < SoAd_SoConArrSize; soconIdx++)
  {
    if((E_OK == soad_IpCmp(&(SoAd_SoConArr[soconIdx].SoAdSocketRemoteIpAddress[0]), ip, SOAD_IPV4_ADD_SIZE)) &&
        SoAd_SoConArr[soconIdx].SoAdSocketRemotePort == port)
    {
      *retSocon = soconIdx;
      ret = E_OK;
      break;
    }
  }

  return ret;
}

static void soad_SocketConnectRoutine(SoAd_SoConIdType *SoConId)
{
  int iResult = -1;
  Std_ReturnType ret = E_OK;

  SoAd_SoConIdType thisSoConId = *SoConId;
  SoAdSoCon_t *thisSoAdSock = &SoAd_DynSoConArr[thisSoConId];

  DWORD threadId;
  HANDLE threadHdl;

  SoAd_CfgSoConGrp_t *soConGrCfg = &SOAD_GET_SOCON_GROUP(thisSoConId);
  SoAd_CfgSoCon_t *soConCfg = &SoAd_SoConArr[thisSoConId];

  struct sockaddr_in clientService;
  clientService.sin_family = soConGrCfg->W32AfType;
  clientService.sin_addr.s_addr = inet_addr(soConCfg->SoAdSocketRemoteIpAddress);
  clientService.sin_port = htons(soConCfg->SoAdSocketRemotePort);

  SOAD_W32_CHECK_ENV(ret);

  if(ret == E_OK)
  {
    while(1)
    {
      iResult = connect(thisSoAdSock->W32Sock, (SOCKADDR *) & clientService, sizeof (clientService));
      if (iResult == SOCKET_ERROR) {
//          iResult = closesocket(thisSoAdSock->W32Sock);
//          if (iResult == SOCKET_ERROR)
//          WSACleanup();
//          return 1;
        Sleep(SOAD_CFG_TCP_CLT_TX_SYNC_DELAY_MS);

      }else
      {
        //create thread for receive client data
        threadHdl = CreateThread( NULL, 1024,
                  ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                  &(thisSoAdSock->W32SoAdSoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl)
        {
          soad_SoConModeChgAllUppers(thisSoConId, SOAD_SOCON_ONLINE);

          thisSoAdSock->W32SockState = SOAD_W32SOCK_STATE_CONNECTED;

          //for soad_SocketRxRoutine
          ResumeThread(threadHdl);

          thisSoAdSock->W32Thread.Id = threadId;
          thisSoAdSock->W32Thread.Hdl = threadHdl;

          //terminate this thread
          break;
        }else
        {
          break;
          //TODO: handle ?
        }
      }
    }
  }
}

static Std_ReturnType soad_IpCmp(char *s1, char *s2, uint32 size)
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
  return ret;
}

static void soad_IfRxIndicationAllUppers(SoAd_SoConIdType SoConId, PduInfoType *Info)
{
  uint32 socketRoutedestCtn = 0;
  uint32 socketRoutedest = 0;
  PduIdType upperRxPduId = 0;

  while(socketRoutedestCtn < SoAd_SoConArr[SoConId].SocketRouteDestListSize)
  {
    socketRoutedest = SoAd_SoConArr[SoConId].SocketRouteDestList[socketRoutedestCtn];

    upperRxPduId = SoAd_SocketRouteArr[socketRoutedestCtn].SoAdRxPduRef;

    SOAD_GET_UPPER_FNCTBL_BY_SOCKETROUTEDEST(socketRoutedest).UpperIfRxIndication(upperRxPduId, Info);

    socketRoutedestCtn++;
  }
}

static void soad_SoConModeChgAllUppers(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode)
{
  uint32 pduroutedestCtn = 0;
  uint32 pduroutedest = 0;

  while(pduroutedestCtn < SoAd_SoConArr[SoConId].PduRouteDestListSize)
  {
    pduroutedest = SoAd_SoConArr[SoConId].PduRouteDestList[pduroutedestCtn];

    SOAD_GET_UPPER_FNCTBL_BY_PDUROUTEDEST(pduroutedest).UpperSoConModeChg(SoConId, Mode);

    SoAd_DynSoConArr[SoConId].SoAdSoConState = Mode;

    pduroutedestCtn++;
  }
}

static void soad_HandleTpRxSession(SoAd_SoConIdType SoConId)
{
  BufReq_ReturnType upperBufReqRet = BUFREQ_E_NOT_OK;
  PduLengthType upperBufferSizeAsked;

  PduLengthType copiedLength = -1;

  PduInfoType pduInfo;

  SoAd_SoConBuffer_t *soConBufferData;

  BufReq_ReturnType upperBufferReqRet;

  SoAd_CfgSocketRoute_t *socketRoute = &SOAD_GET_TCP_SOCON_SOCKET_ROUTE(SoConId);

  switch(SoAd_DynSoConArr[SoConId].RxSsState)
  {
    case SOAD_SS_START:
      soConBufferData = soad_GetFirstElementSoconQueue(SoConId);

      //call <Up>_[SoAd][Tp]StartOfReception to ask size
      pduInfo.SduLength = 0;
      pduInfo.SduDataPtr = NULL_PTR;

      SoAd_DynSoConArr[SoConId].RxSsCopiedLength = soConBufferData->length;

      /* SWS_SoAd_00568  */
      upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpStartOfReception(
          socketRoute->SoAdRxPduRef, &pduInfo, soConBufferData->length, &upperBufferSizeAsked);

      if(upperBufferReqRet == BUFREQ_OK)
      {
        if(upperBufferSizeAsked > 0)
        {
          pduInfo.SduLength = upperBufferSizeAsked;
          pduInfo.SduDataPtr = &(soConBufferData->data[0]);

          copiedLength = (upperBufferSizeAsked >= soConBufferData->length) ?
              soConBufferData->length : upperBufferSizeAsked;

          /* call Up>_[SoAd][Tp]CopyRxData */
          upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpCopyRxData(
              socketRoute->SoAdRxPduRef,
              &pduInfo,
              &upperBufferSizeAsked);

          if(upperBufferReqRet == BUFREQ_OK)
          {

            if(copiedLength == soConBufferData->length)
            {
              //Copy done -> UpperTpRxIndication
              SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpRxIndication(socketRoute->SoAdRxPduRef, E_OK);
              SoAd_DynSoConArr[SoConId].RxSsState = SOAD_SS_DONE;

              soad_RemoveFirstElementSoconQueue(SoConId);
            }else
            {
              //copy on going
              SoAd_DynSoConArr[SoConId].RxSsCopiedLength = copiedLength;
              SoAd_DynSoConArr[SoConId].RxSsLastUpperAskedSize = upperBufferSizeAsked;
              SoAd_DynSoConArr[SoConId].RxSsState = SOAD_SS_COPYING;
            }
          }else
          {
            /* TODO: SWS_SoAd_00570 -> close socket */
          }

        }else
        {
          /* No buffer size in upper, do nothing */
        }
      }else
      {
        /* TODO: SWS_SoAd_00570 -> close socket */
      }

      break;
    case SOAD_SS_COPYING:

      soConBufferData = soad_GetFirstElementSoconQueue(SoConId);

      pduInfo.SduLength = SoAd_DynSoConArr[SoConId].RxSsLastUpperAskedSize;
      pduInfo.SduDataPtr = &(soConBufferData->data[SoAd_DynSoConArr[SoConId].RxSsCopiedLength]);

      upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpCopyRxData(
          socketRoute->SoAdRxPduRef,
          &pduInfo,
          &upperBufferSizeAsked);

      copiedLength = (upperBufferSizeAsked >= soConBufferData->length) ?
          soConBufferData->length : upperBufferSizeAsked;

      if(BUFREQ_OK == upperBufferReqRet)
      {
        if(copiedLength == soConBufferData->length)
        {
          SoAd_DynSoConArr[SoConId].RxSsState = SOAD_SS_DONE;
        }else
        {
          //copy on going
          SoAd_DynSoConArr[SoConId].RxSsCopiedLength = copiedLength;
          SoAd_DynSoConArr[SoConId].RxSsLastUpperAskedSize = upperBufferSizeAsked;
          SoAd_DynSoConArr[SoConId].RxSsState = SOAD_SS_COPYING;
        }
      }else
      {
        //-> close socket
      }

      break;
    case SOAD_SS_DONE:
      //Copy done -> UpperTpRxIndication
      SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpRxIndication(socketRoute->SoAdRxPduRef, E_OK);
      soad_RemoveFirstElementSoconQueue(SoConId);
      SoAd_DynSoConArr[SoConId].RxSsState = SOAD_SS_STOP;
      break;
    case SOAD_SS_STOP:
      break;
    default:
      break;
  }

  if(upperBufferReqRet == BUFREQ_E_NOT_OK)
  {
    /* TODO: SWS_SoAd_00570 -> close socket */
    SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
  }
}

static void soad_EnQueueSoconBuffer(SoAd_SoConIdType SoConId, SoAd_SoConBuffer_t *SoConData)
{

}

static SoAd_SoConBuffer_t *soad_GetFirstElementSoconQueue(SoAd_SoConIdType SoConId)
{

}

static void soad_RemoveFirstElementSoconQueue(SoAd_SoConIdType SoConId)
{

}
/* ***************************** [ FUNCTIONS ] ****************************** */
