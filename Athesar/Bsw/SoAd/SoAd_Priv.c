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

static void soad_HandleTpRxSession(SoAd_SoConIdType SoConId);
static void soad_HandleTpTxSession(SoAd_SoConIdType SoConId);
static Std_ReturnType soad_SendSoCon(SoAd_SoConIdType SoConId, PduInfoType *PduInfo);

static void soad_InitializeSoConQueue(SoAd_SocketBufferQueue_t *queue);
static boolean soad_isSoConQueueFull(SoAd_SocketBufferQueue_t *queue);
static boolean soad_isQueueEmpty(SoAd_SocketBufferQueue_t *queue);
static boolean soad_EnqueueSoConData(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *buffer);
static boolean soad_getSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *dest);
static boolean soad_RemoveSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue);
/* ***************************** [ DATAS     ] ****************************** */

SoAd_SoCon_t SoAd_DynSoConArr[SOAD_CFG_NUM_SOCON];
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
    if(SOAD_IS_SOCON_DATA_ONGOING(SoConId))
    {
      /*TODO: handle on going TX/RX data */
    }else
    {
      /* free SoCon connection resource */
      soad_FreeSoCon(SoConId);
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
    }
  }

  SOAD_UNUSED(dummy);
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

  _SoAd_InitSocon(SoConId);
}

void _SoAd_InitSocon(SoAd_SoConIdType SoConId)
{
  SoAd_SoCon_t *soCon = &SoAd_DynSoConArr[SoConId];

  memset(soCon->RemoteAddress, 0, SOAD_IPV4_ADD_SIZE);

  soCon->RemotePort = 0;

  soCon->RequestMask = SOAD_SOCCON_REQMASK_NON;

  soad_InitializeSoConQueue(&(soCon->RxQueue));
  soad_InitializeSoConQueue(&(soCon->TxQueue));

  soCon->RxSsState = SOAD_SS_STOP;
  soCon->TxSsState = SOAD_SS_STOP;

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
  soad_HandleTpRxSession(SoConId);

  soad_HandleTpTxSession(SoConId);
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

static Std_ReturnType soad_SendSoCon(SoAd_SoConIdType SoConId, PduInfoType *PduInfo)
{
  Std_ReturnType ret = E_OK;

  const SoAd_CfgSoConGrp_t *thisSoConGrCfg;
  const SoAd_CfgSoCon_t *thisSoConCfg;

  int iResult = -1;
  struct sockaddr_in recvAddr;

  thisSoConGrCfg = &SOAD_GET_SOCON_GROUP(SoConId);
  thisSoConCfg = &SoAd_SoConArr[SoConId];

  SOCKET sendSocket = SoAd_DynSoConArr[SoConId].W32Sock;

  if(SoAd_DynSoConArr[SoConId].SoAdSoConState == SOAD_SOCON_ONLINE)
  {
    if(SOAD_IS_UDP_SOCON(SoConId))
    {
      recvAddr.sin_family = thisSoConGrCfg->W32AfType;
      recvAddr.sin_port = htons(thisSoConCfg->RemotePort);
      recvAddr.sin_addr.s_addr = inet_addr(thisSoConCfg->RemoteIpAddress);

      iResult = sendto(
          sendSocket, (char *)(PduInfo->SduDataPtr),
          PduInfo->SduLength, 0,  (SOCKADDR *)&recvAddr, sizeof(recvAddr));

      if (iResult == SOCKET_ERROR)
      {
          ret = E_NOT_OK;
      }
    }else
    {
      //TCP
      iResult = send( sendSocket, (char *)(PduInfo->SduDataPtr), PduInfo->SduLength, 0 );

      if (iResult == SOCKET_ERROR)
      {
          ret = E_NOT_OK;
      }
    }
  }else
  {
    ret = E_NOT_OK;
  }

  return ret;
}


Std_ReturnType _SoAd_IfPduFanOut(PduIdType TxPduId, const PduInfoType *PduInfo)
{
  int iResult = -1;

  Std_ReturnType retLogic = E_OK;
  Std_ReturnType retTx = E_OK;

  const SoAd_CfgPduRouteDest_t *SoAdPduRouteDest;

  const SoAd_CfgPduRoute_t *pduRoute;

  uint32 SoAdPduRouteDestIdx;

  pduRoute = &SoAd_PduRouteArr[TxPduId];

  SoAd_SoConIdType soConIdx;

  if(retLogic == E_OK)
  {
    SoAdPduRouteDestIdx = pduRoute->SoAdPduRouteDestBase;
    while((SoAdPduRouteDestIdx < (pduRoute->SoAdPduRouteDestBase + pduRoute->SoAdPduRouteDestCtn)) &&
        retLogic == E_OK)
    {
      SoAdPduRouteDest = &SoAd_PduRouteDestArr[SoAdPduRouteDestIdx];

      //send to GROUP that PduDest refer to
      if(SoAdPduRouteDest->SoAdTxSoConGrIdx != SOAD_INVALID_SOCON_GROUP)
      {
        //send on All active SOCON in group
        soConIdx = 0;
        while(soConIdx < SoAd_SoConArrSize)
        {
          if(SoAd_DynSoConArr[soConIdx].SoAdSoConState == SOAD_SOCON_ONLINE)
          {
            //check that SOCON belongs to this group
            if(SoAd_SoConArr[soConIdx].SoConGrIdx == SoAdPduRouteDest->SoAdTxSoConGrIdx)
            {
              retTx = soad_SendSoCon(SoAdPduRouteDest->SoAdTxSoConIdx, PduInfo);
            }
          }
          soConIdx++;
        }
      }else if(SoAdPduRouteDest->SoAdTxSoConIdx != SOAD_INVALID_SOCON)
      {
        retTx = soad_SendSoCon(SoAdPduRouteDest->SoAdTxSoConIdx, PduInfo);
      }else
      {
        //No SOCON reference, NO SOCON GROUP reference -> invalid CONFIG
        retLogic = E_NOT_OK;
      }

      if(retLogic == E_OK)
      {
        SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperIfTxConfirmation(TxPduId, retTx);
      }

      SoAdPduRouteDestIdx++;
    }
  }

  return retLogic;
}

//static function
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
      threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
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
      threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
                ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                &SoAd_DynSoConArr[SoConId].W32SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                &threadId );

      if(threadHdl != INVALID_HANDLE_VALUE)
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
      threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
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

  const SoAd_CfgSoConGrp_t *soConGrCfg = &SOAD_GET_SOCON_GROUP(SoConId);
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

  const SoAd_CfgSoConGrp_t *soConGrCfg = &SoAd_SoConGrArr[SoConId];
  SoAd_SoCon_t *thisSoCon = &SoAd_DynSoConArr[SoConId];

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
  SoAd_SoCon_t *thisSoSon = &SoAd_DynSoConArr[thisSoConId];

  PduInfoType pduInfo;
  uint8 rxBuffer[SOAD_CFG_SOCON_TRX_BUFF_SIZE];

  SoAd_SoConBuffer_t buffData;

  while(1)
  {
    memset(&rxBuffer[0], 0, SOAD_CFG_SOCON_TRX_BUFF_SIZE);
    resultLength = recv(thisSoSon->W32Sock, (char *)&rxBuffer[0], SOAD_CFG_SOCON_TRX_BUFF_SIZE, 0);

    if(resultLength != -1)
    {
      if(SOAD_IS_UDP_SOCON(thisSoConId))
      {
        pduInfo.SduLength = resultLength;
        pduInfo.SduDataPtr = &rxBuffer[0];

        soad_IfRxIndicationAllUppers(thisSoConId, &pduInfo);

        thisSoSon->RxSsCopiedLength = 0;

      }else if(SOAD_IS_TCP_SOCON(thisSoConId))
      {
        if(resultLength != 0)
        {
          //upper is IF
          if(SOAD_GET_TCP_SOCON_SOCKET_ROUTE(thisSoConId).SoAdRxUpperLayerType == SOAD_UPPER_IF)
          {
            pduInfo.SduLength = resultLength;
            pduInfo.SduDataPtr = &rxBuffer[0];
            soad_IfRxIndicationAllUppers(thisSoConId, &pduInfo);
          }else
          {
            //upper is TP
            buffData.length = resultLength;
            memcpy(&(buffData.data[0]), &rxBuffer[0], resultLength);

            //store queue
            soad_EnqueueSoConData(&(thisSoSon->RxQueue), &buffData);
          }
        }else
        {
          //TODO: when connection lost detected
          if(SOAD_IS_TCP_SERVER_SOCON(thisSoConId))
          {
            thisSoSon->W32SockState = SOAD_W32SOCK_STATE_LISTENING;
            soad_SoConModeChgAllUppers(thisSoConId, SOAD_SOCON_RECONNECT);
          }else
          {
            //case TCP client connection
            thisSoSon->W32SockState = SOAD_W32SOCK_STATE_INVALID;
            soad_SoConModeChgAllUppers(thisSoConId, SOAD_SOCON_OFFLINE);
          }

          closesocket(thisSoSon->W32Sock);
          TerminateThread(thisSoSon->W32Thread.Hdl, 0);
        }
      }else
      {
        SOAD_LOG("Invalid protocol!!!");
      }
    }else
    {
      //Exceed SOAD_CFG_SOCON_RX_BUFF_SIZE, TODO: handle ?
      SOAD_LOG("Exceed SOAD_CFG_SOCON_RX_BUFF_SIZE");
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
  SoAd_SoCon_t *thisSoAdSock;

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
        threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
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
          SOAD_LOG("failed to create thread");
          break;
        }
      }
    }else
    {
      SOAD_LOG_PAR("Refuse Connection from: %s", inet_ntoa(addr.sin_addr));
      SOAD_LOG_PAR("Refuse Connection from: %d", ntohs(addr.sin_port));
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
    if((E_OK == soad_IpCmp(&(SoAd_SoConArr[soconIdx].RemoteIpAddress[0]), ip, SOAD_IPV4_ADD_SIZE)) &&
        SoAd_SoConArr[soconIdx].RemotePort == port)
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
  SoAd_SoCon_t *thisSoAdSock = &SoAd_DynSoConArr[thisSoConId];

  DWORD threadId;
  HANDLE threadHdl;

  const SoAd_CfgSoConGrp_t *soConGrCfg = &SOAD_GET_SOCON_GROUP(thisSoConId);
  const SoAd_CfgSoCon_t *soConCfg = &SoAd_SoConArr[thisSoConId];

  struct sockaddr_in clientService;
  clientService.sin_family = soConGrCfg->W32AfType;
  clientService.sin_addr.s_addr = inet_addr(soConCfg->RemoteIpAddress);
  clientService.sin_port = htons(soConCfg->RemotePort);

  SOAD_W32_CHECK_ENV(ret);

  if(ret == E_OK)
  {
    while(1)
    {
      iResult = connect(thisSoAdSock->W32Sock, (SOCKADDR *) & clientService, sizeof (clientService));
      if (iResult == SOCKET_ERROR) {

        Sleep(SOAD_CFG_TCP_CLT_TX_SYNC_DELAY_MS);

      }else
      {
        //create thread for receive client data
        threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
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
          SOAD_LOG("failed to create thread");
          break;
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
  uint32 pdurouteCtn = 0;
  uint32 pduroute = 0;

  while(pdurouteCtn < SoAd_SoConArr[SoConId].PduRouteSize)
  {
    pduroute = SoAd_SoConArr[SoConId].PduRouteList[pdurouteCtn];

    SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(pduroute).UpperSoConModeChg(SoConId, Mode);

    pdurouteCtn++;
  }

  SoAd_DynSoConArr[SoConId].SoAdSoConState = Mode;
}

static void soad_HandleTpTxSession(SoAd_SoConIdType SoConId)
{

}

static void soad_HandleTpRxSession(SoAd_SoConIdType SoConId)
{
  PduLengthType upperBufferSizeAsked;

  PduInfoType pduInfo;

  SoAd_SoConBuffer_t soConBufferData;

  BufReq_ReturnType upperBufferReqRet = BUFREQ_OK;

  const SoAd_CfgSocketRoute_t *socketRoute = &SOAD_GET_TCP_SOCON_SOCKET_ROUTE(SoConId);

  SoAd_SoCon_t *thisSoCon = &SoAd_DynSoConArr[SoConId];

  if(thisSoCon->RxSsState == SOAD_SS_STOP)
  {
    if(soad_getSoConQueueFirstElement(&(thisSoCon->RxQueue), &soConBufferData) == SOAD_TRUE)
    {
      thisSoCon->RxSsState = SOAD_SS_START;
    }
  }

  switch((thisSoCon->RxSsState))
  {
    case SOAD_SS_START:
      //call <Up>_[SoAd][Tp]StartOfReception to ask size
      pduInfo.SduLength = 0;
      pduInfo.SduDataPtr = NULL_PTR;

      thisSoCon->RxSsCopiedLength = 0;
      thisSoCon->RxSsLastUpperAskedSize = 0;

      /* SWS_SoAd_00568  */
      upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpStartOfReception(
          socketRoute->SoAdRxPduRef, &pduInfo, soConBufferData.length, &upperBufferSizeAsked);

      if(upperBufferReqRet == BUFREQ_OK)
      {
        if(upperBufferSizeAsked > 0)
        {
          pduInfo.SduLength = (upperBufferSizeAsked >= soConBufferData.length) ? soConBufferData.length : upperBufferSizeAsked;
          pduInfo.SduDataPtr = &(soConBufferData.data[0]);

          thisSoCon->RxSsCopiedLength += pduInfo.SduLength;

          /* call Up>_[SoAd][Tp]CopyRxData */
          upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpCopyRxData(
              socketRoute->SoAdRxPduRef,
              &pduInfo,
              &upperBufferSizeAsked);

          if(upperBufferReqRet == BUFREQ_OK)
          {

            if(thisSoCon->RxSsCopiedLength == soConBufferData.length)
            {
              //Copy done
              thisSoCon->RxSsState = SOAD_SS_DONE;
            }else
            {
              //copy on going
              thisSoCon->RxSsLastUpperAskedSize = upperBufferSizeAsked;
              thisSoCon->RxSsState = SOAD_SS_COPYING;
            }
          }else
          {
            /* TODO: SWS_SoAd_00570 -> close socket */
          }

        }else
        {
          /* No buffer size in upper, do nothing */
          SOAD_LOG("No buffer size in upper");
        }
      }else
      {
        /* TODO: SWS_SoAd_00570 -> close socket */
      }

      break;
    case SOAD_SS_COPYING:
      soad_getSoConQueueFirstElement(&(thisSoCon->RxQueue), &soConBufferData);

      pduInfo.SduLength = ((soConBufferData.length - thisSoCon->RxSsCopiedLength) >= thisSoCon->RxSsLastUpperAskedSize) ? \
          thisSoCon->RxSsLastUpperAskedSize : (soConBufferData.length - thisSoCon->RxSsCopiedLength);

      pduInfo.SduDataPtr = &(soConBufferData.data[thisSoCon->RxSsCopiedLength]);

      upperBufferReqRet = SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpCopyRxData(
          socketRoute->SoAdRxPduRef,
          &pduInfo,
          &upperBufferSizeAsked);

      thisSoCon->RxSsCopiedLength += pduInfo.SduLength;

      if(BUFREQ_OK == upperBufferReqRet)
      {
        if(thisSoCon->RxSsCopiedLength == soConBufferData.length)
        {
          thisSoCon->RxSsState = SOAD_SS_DONE;
        }else
        {
          //copy on going
          thisSoCon->RxSsLastUpperAskedSize = upperBufferSizeAsked;
          thisSoCon->RxSsState = SOAD_SS_COPYING;
        }
      }else
      {
        //-> close socket
      }

      break;
    case SOAD_SS_DONE:
      //Copy done -> UpperTpRxIndication
      SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpRxIndication(socketRoute->SoAdRxPduRef, E_OK);
      soad_RemoveSoConQueueFirstElement(&(thisSoCon->RxQueue));
      thisSoCon->RxSsState = SOAD_SS_STOP;
      break;
    case SOAD_SS_STOP:
      break;
    default:
      break;
  }

  if(upperBufferReqRet == BUFREQ_E_NOT_OK)
  {
    /* SWS_SoAd_00570 -> close socket */
    SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
  }
}

void soad_InitializeSoConQueue(SoAd_SocketBufferQueue_t *queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

boolean soad_isSoConQueueFull(SoAd_SocketBufferQueue_t *queue) {
    return queue->size == SOAD_SOCON_QUEUE_DEPTH;
}

boolean soad_isQueueEmpty(SoAd_SocketBufferQueue_t *queue) {
    return queue->size == 0;
}

boolean soad_EnqueueSoConData(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *buffer) {
    if (soad_isSoConQueueFull(queue)) {
        return FALSE; // Queue is full, unable to enqueue
    }

    queue->rear = (queue->rear + 1) % SOAD_SOCON_QUEUE_DEPTH;
    queue->Buffer[queue->rear] = *buffer;
    queue->size++;
    return TRUE;
}

boolean soad_getSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *dest) {
    if (soad_isQueueEmpty(queue)) {
        return FALSE; // Queue is empty, unable to get first element
    }

    *dest = queue->Buffer[queue->front];
    return TRUE;
}

boolean soad_RemoveSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue) {
    if (soad_isQueueEmpty(queue)) {
        return FALSE; // Queue is empty, unable to remove first element
    }

    queue->front = (queue->front + 1) % SOAD_SOCON_QUEUE_DEPTH;
    queue->size--;
    return TRUE;
}
/* ***************************** [ FUNCTIONS ] ****************************** */
