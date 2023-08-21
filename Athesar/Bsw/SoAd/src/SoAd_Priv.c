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
static void soad_SocketListenRoutine(uint32 *SoConGrIdx);

static void soad_SoConModeChgAllUppers(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode);
static void soad_IfRxIndicationAllUppers(SoAd_SoConIdType SoConId, PduInfoType *Info);
static void soad_SoConIpAssignChgNotifAllUppers(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State);
static void soad_TpRxIndicationAllUppers(SoAd_SoConIdType SoConId, Std_ReturnType Result);

static Std_ReturnType soad_CreateSocket(SoAd_SoConIdType SoConId);
static Std_ReturnType soad_BindSocket(SoAd_SoConIdType SoConId);
static void soad_FreeSoCon(SoAd_SoConIdType SoConId);

static Std_ReturnType soad_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon, uint32 SoConGrIdx);

static boolean soad_IsFanOutPDU(PduIdType TxPduId);

static Std_ReturnType soad_IpCmp(char *s1, char *s2, uint32 size);
static boolean soad_isMulticastIpv4Addr(const char *ip_address);

static void soad_HandleTpRxSession(SoAd_SoConIdType SoConId);
static void soad_HandleTpTxSession(SoAd_SoConIdType SoConId);
static Std_ReturnType soad_SendSoCon(SoAd_SoConIdType SoConId, const PduInfoType *PduInfo);

static void soad_InitSoConQueue(SoAd_SocketBufferQueue_t *queue);
static boolean soad_EnqueueSoConData(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *buffer);
static boolean soad_getSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *dest);
static boolean soad_RemoveSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue);
/* ***************************** [ DATAS     ] ****************************** */

/* ***************************** [ LOCALS    ] ****************************** */


void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId)
{
  uint8 dummy;
  switch(SoAd_DynSoConArr[SoConId].W32SockState)
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
      dummy = SoAd_DynSoConArr[SoConId].W32SockState;
      break;
    case SOAD_W32SOCK_STATE_CONNECTED:
      dummy = SoAd_DynSoConArr[SoConId].W32SockState;
      break;
    case SOAD_W32SOCK_STATE_LISTENING:
      dummy = SoAd_DynSoConArr[SoConId].W32SockState;
      break;
    case SOAD_W32SOCK_STATE_ACCEPTED:
      dummy = SoAd_DynSoConArr[SoConId].W32SockState;
      break;
    default:
      break;
  }

  /* SWS_SoAd_00588, SWS_SoAd_00604, SWS_SoAd_00642 */
  if(SOAD_CHECK_SOCON_NEED_CLOSE(SoConId))
  {
    if((SoAd_DynSoConArr[(SoConId)].RxSsState != SOAD_SS_STOP))
    {
      /* SWS_SoAd_00637 */
      /* (1) Terminate active TP sessions (if any) and notify the upper layer about the termination */
      SoAd_DynSoConArr[(SoConId)].RxSsState = SOAD_SS_DONE;
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
  SoAd_SoCon_t *soCon = &SoAd_DynSoConArr[SoConId];

  soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_OFFLINE);

  TerminateThread(SoAd_DynSoConArr[SoConId].W32Thread.Hdl, 0);

  soad_CloseSoCon(SoConId);

  memcpy(&(soCon->RemoteAddress[0]), &(SoAd_SoConArr[SoConId].RemoteIpAddress[0]) , SOAD_IPV4_ADD_SIZE);

  soCon->RemotePort = SoAd_SoConArr[SoConId].RemotePort;

  soCon->RequestMask = SOAD_SOCCON_REQMASK_NON;

  soad_InitSoConQueue(&(soCon->RxQueue));

  soCon->RxSsState = SOAD_SS_STOP;
  soCon->RxSsCopiedLength = 0;

  soCon->SoAdSoConState = SOAD_SOCON_OFFLINE;

  soCon->W32Sock = SOAD_W32_INVALID_SOCKET;

  soCon->W32SockState = SOAD_W32SOCK_STATE_INVALID;

  soCon->W32Thread.Hdl = INVALID_HANDLE_VALUE;
  soCon->W32Thread.Id = SOAD_INVALID_NUMBER;
}

void _SoAd_InitSocon(SoAd_SoConIdType SoConId)
{
  SoAd_SoCon_t *soCon = &SoAd_DynSoConArr[SoConId];

  memcpy(&(soCon->RemoteAddress[0]), &(SoAd_SoConArr[SoConId].RemoteIpAddress[0]) , SOAD_IPV4_ADD_SIZE);

  soCon->RemotePort = SoAd_SoConArr[SoConId].RemotePort;

  soCon->RequestMask = SOAD_SOCCON_REQMASK_NON;

  soad_InitSoConQueue(&(soCon->RxQueue));
  soad_InitSoConQueue(&(soCon->TxQueue));

  soCon->RxSsState = SOAD_SS_STOP;
  soCon->TxSsState = SOAD_SS_STOP;

  soCon->RxSsCopiedLength = 0;

  soCon->SoAdSoConState = SOAD_SOCON_OFFLINE;
  soCon->IpAddrState = TCPIP_IPADDR_STATE_UNASSIGNED;

  soCon->W32Sock = SOAD_W32_INVALID_SOCKET;

  soCon->W32SockState = SOAD_W32SOCK_STATE_INVALID;

  soCon->W32Thread.Hdl = INVALID_HANDLE_VALUE;
  soCon->W32Thread.Id = SOAD_INVALID_NUMBER;

  //this stuff is for pass over to the thread.
  soCon->SoAdSoConId = SoConId;
}

void _SoAd_InitSoConGroup(uint32 SoConGr)
{
  SoAd_SoConGr_t *soConGr = &SoAd_DynSoConGrArr[SoConGr];

  soConGr->W32SockListen = SOAD_W32_INVALID_SOCKET;

  soConGr->W32SockListenState = SOAD_W32SOCK_STATE_INVALID;

  soConGr->W32Thread.Hdl = INVALID_HANDLE_VALUE;

  soConGr->W32Thread.Id = SOAD_INVALID_NUMBER;
}

void _SoAd_HandleRxData(SoAd_SoConIdType SoConId)
{
  soad_HandleTpRxSession(SoConId);
}

void _SoAd_HandleTxData(PduIdType TxPduId)
{
  SoAd_SoConIdType relatedSoCon = SOAD_INVALID_SOCON;

  /* SWS_SoAd_00588, SWS_SoAd_00604 */
  if(soad_IsFanOutPDU(TxPduId) == SOAD_FALSE)
  {
    relatedSoCon = SOAD_GET_SOCONID_BY_TXPDU(TxPduId);
    if(SOAD_CHECK_SOCON_NEED_CLOSE(relatedSoCon))
    {
      if(SoAd_DynTxPdu[TxPduId].TxSsState != SOAD_SS_STOP)
      {
        /* handle on going TX/RX data */
        /* SWS_SoAd_00637 */
        /* (1) Terminate active TP sessions (if any) and notify the upper layer about the termination */
        SoAd_DynTxPdu[TxPduId].TxSsState = SOAD_SS_DONE;
      }
    }
  }

  soad_HandleTpTxSession(TxPduId);
}

static void soad_HandleSoConStateInvalid(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;
  static SoAd_SoConIdType SoCon1stEnterCtn = 0;

  if(SoCon1stEnterCtn < SoAd_SoConArrSize)
  {
    SoCon1stEnterCtn++;

    soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_OFFLINE);
    //stub <Up>_LocalIpAddrAssignmentChg, SWS_SoAd_00598
    soad_SoConIpAssignChgNotifAllUppers(SoConId, TCPIP_IPADDR_STATE_ASSIGNED);
  }

  /* SWS_SoAd_00588 */
  /* SWS_SoAd_00589 */
  if((SOAD_CHECK_SOCON_NEED_OPEN(SoConId) || SOAD_IS_AUTO_SOCON(SoConId)) &&
      SOAD_IS_IP_ASSIGNED(SoConId))
  {
    /* SWS_SoAd_00590 */
    ret = soad_CreateSocket(SoConId);

    if(ret == E_OK)
    {
      SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_NEW;
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);

    }else
    {
      SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_INVALID;
    }
  }
}

static Std_ReturnType soad_SendSoCon(SoAd_SoConIdType SoConId, const PduInfoType *PduInfo)
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

      /* SWS_SoAd_00540 */
      iResult = sendto(
          sendSocket, (char *)(PduInfo->SduDataPtr),
          PduInfo->SduLength, 0,  (SOCKADDR *)&recvAddr, sizeof(recvAddr));

      if (iResult == SOCKET_ERROR)
      {
          ret = E_NOT_OK;
      }
    }else
    {
      //TCP, SWS_SoAd_00542
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

      SoAdPduRouteDestIdx++;
    }
  }

  if(retLogic == E_OK)
  {
    /* SWS_SoAd_00544, SWS_SoAd_00545 */
    if(SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperIfTxConfirmation != NULL_PTR)
    {
      SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperIfTxConfirmation(TxPduId, retTx);
    }
  }

  return retLogic;
}


Std_ReturnType _SoAd_RequestTpTxSs(PduIdType TxPduId, const PduInfoType *PduInfo)
{
  Std_ReturnType ret = E_NOT_OK;

  if(SoAd_DynTxPdu[TxPduId].TxSsState == SOAD_SS_STOP)
  {
    if(soad_IsFanOutPDU(TxPduId) == SOAD_FALSE)
    {
      if(SOAD_GET_DYN_SOCON_BY_TXPDU(TxPduId).SoAdSoConState == SOAD_SOCON_ONLINE)
      {
        SoAd_DynTxPdu[TxPduId].TxSsState = SOAD_SS_START;
        ret = E_OK;
      }
    }
  }

  return ret;
}


//static function
static boolean soad_IsFanOutPDU(PduIdType TxPduId)
{
  boolean ret = SOAD_FALSE;

  if(SoAd_PduRouteArr[TxPduId].SoAdPduRouteDestCtn == 1)
  {
    if(SoAd_PduRouteDestArr[SoAd_PduRouteArr[TxPduId].SoAdPduRouteDestBase].SoAdTxSoConGrIdx == SOAD_INVALID_SOCON_GROUP)
    {
      if(SoAd_PduRouteDestArr[SoAd_PduRouteArr[TxPduId].SoAdPduRouteDestBase].SoAdTxSoConIdx == SOAD_INVALID_SOCON)
      {
        //NO socket Configured
        ret = SOAD_TRUE;
      }
    }else
    {
      ret = SOAD_TRUE;
    }
  }else
  {
    //FAN out PDU or INVALID CONFIG
    ret = SOAD_TRUE;
  }

  return ret;
}

static void soad_HandleSoConStateNew(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_NOT_OK;

  /* SWS_SoAd_00590 */
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

  struct ip_mreq mreq;

  if(SOAD_IS_TCP_SERVER_SOCON(SoConId))
  {
    if(SOAD_GET_DYN_SOCON_GROUP(SoConId).W32SockListenState == SOAD_W32SOCK_STATE_BIND)
    {
      /* SWS_SoAd_00638, TCP server */
      threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
                ( LPTHREAD_START_ROUTINE ) soad_SocketListenRoutine,
                &SOAD_GET_SOCON_GROUPID(SoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
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
    {
      /* SWS_SoAd_00639, create thread for receive UDP data */
      if(SOAD_IS_MULTICAST_SOCON(SoConId))
      {
        mreq.imr_multiaddr.s_addr = inet_addr(SOAD_GET_SOCON_GROUP(SoConId).W32LocalAddress);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);

        if (setsockopt(SoAd_DynSoConArr[SoConId].W32Sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
            (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR)
        {
            SOAD_LOG("Join multicast group failed.\n");
            ret = E_NOT_OK;
        }
      }

      if(ret == E_OK)
      {
        threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
                  ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                  &SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl != INVALID_HANDLE_VALUE)
        {
          SoAd_DynSoConArr[SoConId].W32Thread.Id = threadId;
          SoAd_DynSoConArr[SoConId].W32Thread.Hdl = threadHdl;

          /* SWS_SoAd_00591 */
          soad_SoConModeChgAllUppers(SoConId, SOAD_SOCON_ONLINE);

          SoAd_DynSoConArr[SoConId].W32SockState = SOAD_W32SOCK_STATE_CONNECTED;

          ResumeThread(threadHdl);
        }else
        {
          ret = E_NOT_OK;
        }
      }
    }else
    {
      /* SWS_SoAd_00590, TCP client, create thread for Connect */
      threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
                ( LPTHREAD_START_ROUTINE ) soad_SocketConnectRoutine,
                &SoAd_DynSoConArr[SoConId].SoAdSoConId, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
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
  SoAd_SoConGr_t *soConGr = &SOAD_GET_DYN_SOCON_GROUP(SoConId);

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
        SoAd_DynSoConArr[SoConId].W32Sock = newSock;
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
      if(SOAD_IS_MULTICAST_SOCON(SoConId))
      {
        sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      }

      iResult = bind(thisSoCon->W32Sock, (SOCKADDR *) &sockaddr, sizeof (sockaddr));
      if (iResult == SOCKET_ERROR) {
          ret = E_NOT_OK;
      }
    }
  }

  return ret;
}

/* SWS_SoAd_00657, SWS_SoAd_00564, SWS_SoAd_00565 */
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

        /* SWS_SoAd_00567 */
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

            /* SWS_SoAd_00567 */
            soad_IfRxIndicationAllUppers(thisSoConId, &pduInfo);
          }else
          {
            //upper is TP
            buffData.length = resultLength;
            memcpy(&(buffData.data[0]), &rxBuffer[0], resultLength);

            //SWS_SoAd_00568, SWS_SoAd_00569, SWS_SoAd_00570
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
      SOAD_LOG("OR Reject due to Same UDP SoCon");
    }
  }
}

static void soad_SocketListenRoutine(uint32 *SoConGrIdx)
{
  int iResult;
  WSADATA wsaData;

  SOCKADDR_IN addr;
  int addrlen = sizeof(addr);

  DWORD threadId;
  HANDLE threadHdl;

  Std_ReturnType ret = E_NOT_OK;

  SoAd_SoConIdType acceptedSoConId = SOAD_INVALID_SOCON;
  SoAd_SoCon_t *thisSoCon = NULL_PTR;
  SoAd_CfgSoCon_t *thisSoConCfg = NULL_PTR;
  SoAd_SoConIdType grSoConIdx = SOAD_INVALID_SOCON_GROUP;

  SoAd_SoConGr_t *SoConGr = &SoAd_DynSoConGrArr[*SoConGrIdx];

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

    ret = soad_FindMatchSocket(&addr, &acceptedSoConId, *SoConGrIdx);
    if(ret == E_OK)
    {
      thisSoCon = &SoAd_DynSoConArr[acceptedSoConId];

      if(thisSoCon->W32SockState == SOAD_W32SOCK_STATE_LISTENING)
      {
        memcpy(&(thisSoCon->RemoteAddress[0]), inet_ntoa(addr.sin_addr), SOAD_IPV4_ADD_SIZE);
        thisSoCon->RemotePort = ntohs(addr.sin_port);

        thisSoCon->W32Sock = acceptSocket;

        //create thread for receive client data
        threadHdl = CreateThread( NULL, SOAD_SOCON_THREAD_STACK,
                  ( LPTHREAD_START_ROUTINE ) soad_SocketRxRoutine,
                  &(thisSoCon->SoAdSoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl)
        {
          thisSoCon->W32Thread.Id = threadId;
          thisSoCon->W32Thread.Hdl = threadHdl;

          /* SWS_SoAd_00594 */
          soad_SoConModeChgAllUppers(acceptedSoConId, SOAD_SOCON_ONLINE);

          thisSoCon->W32SockState = SOAD_W32SOCK_STATE_ACCEPTED;
          ResumeThread(threadHdl);
        }else
        {
          SOAD_LOG("failed to create thread");
          break;
        }
      }else
      {
        SOAD_LOG_PAR("Refuse Connection from: %s", inet_ntoa(addr.sin_addr));
        SOAD_LOG_PAR("Refuse Connection from: %d", ntohs(addr.sin_port));
        closesocket(acceptSocket);
      }
    }else
    {
      SOAD_LOG_PAR("Refuse Connection from: %s", inet_ntoa(addr.sin_addr));
      SOAD_LOG_PAR("Refuse Connection from: %d", ntohs(addr.sin_port));
      closesocket(acceptSocket);
    }
  }
}

/* SWS_SoAd_00680 */
static Std_ReturnType soad_FindMatchSocket(SOCKADDR_IN *addr, SoAd_SoConIdType *retSocon, uint32 SoConGrIdx)
{
  SoAd_SoConIdType soconIdx = 0;
  SoAd_SoConIdType foundSoconIdx = 0;
  Std_ReturnType ret = E_NOT_OK;

  char *ip = inet_ntoa(addr->sin_addr);
  uint32 port =  ntohs(addr->sin_port);

  for(soconIdx = 0; soconIdx < SoAd_SoConArrSize; soconIdx++)
  {
    if((E_OK == soad_IpCmp(&(SoAd_DynSoConArr[soconIdx].RemoteAddress[0]), ip, SOAD_IPV4_ADD_SIZE)) &&
        (SoAd_DynSoConArr[soconIdx].RemotePort == port) &&
        (SoConGrIdx = SoAd_SoConArr[SoConGrIdx].SoConGrIdx) &&
        (SoAd_DynSoConArr[soconIdx].SoAdSoConState != SOAD_SOCON_ONLINE))
    {
      /* (a) IP address and port match */
      foundSoconIdx = soconIdx;
      soconIdx = SoAd_SoConArrSize;
      ret = E_OK;
    }else if((E_OK == soad_IpCmp(&(SoAd_DynSoConArr[soconIdx].RemoteAddress[0]), ip, SOAD_IPV4_ADD_SIZE)) &&
        (SoAd_DynSoConArr[soconIdx].RemotePort == SOAD_ANY_PORT) &&
        (SoConGrIdx = SoAd_SoConArr[SoConGrIdx].SoConGrIdx) &&
        (SoAd_DynSoConArr[soconIdx].SoAdSoConState != SOAD_SOCON_ONLINE))
    {
      /* (b) IP address match (and wildcard set for port) */
      foundSoconIdx = soconIdx;
      soconIdx = SoAd_SoConArrSize;
      ret = E_OK;

    }else if((E_OK == soad_IpCmp(&(SoAd_DynSoConArr[soconIdx].RemoteAddress[0]), SOAD_ANY_IP, SOAD_IPV4_ADD_SIZE)) &&
        (SoAd_DynSoConArr[soconIdx].RemotePort == port) &&
        (SoConGrIdx = SoAd_SoConArr[SoConGrIdx].SoConGrIdx) &&
        (SoAd_DynSoConArr[soconIdx].SoAdSoConState != SOAD_SOCON_ONLINE))
    {
      /* (c) Port match (and wildcard set for IP address) */
      foundSoconIdx = soconIdx;
      soconIdx = SoAd_SoConArrSize;
      ret = E_OK;
    }else if((E_OK == soad_IpCmp(&(SoAd_DynSoConArr[soconIdx].RemoteAddress[0]), SOAD_ANY_IP, SOAD_IPV4_ADD_SIZE)) &&
        (SoAd_DynSoConArr[soconIdx].RemotePort == SOAD_ANY_PORT) &&
        (SoConGrIdx = SoAd_SoConArr[SoConGrIdx].SoConGrIdx) &&
        (SoAd_DynSoConArr[soconIdx].SoAdSoConState != SOAD_SOCON_ONLINE))
    {
      /* (c) Port match (and wildcard set for IP address) */
      foundSoconIdx = soconIdx;
      soconIdx = SoAd_SoConArrSize;
      ret = E_OK;
    }else
    {
      /* (e) No match (i.e. no socket connections can be selected) */
    }
  }

  if(ret == E_OK)
  {
    *retSocon = foundSoconIdx;
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
                  &(thisSoAdSock->SoAdSoConId), CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
                  &threadId );

        if(threadHdl)
        {
          /* SWS_SoAd_00593, SWS_SoAd_00636 */
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

static void soad_TpRxIndicationAllUppers(SoAd_SoConIdType SoConId, Std_ReturnType Result)
{
  uint32 socketRoutedestCtn = 0;
  uint32 socketRoutedest = 0;
  PduIdType upperRxPduId = 0;

  while(socketRoutedestCtn < SoAd_SoConArr[SoConId].SocketRouteDestListSize)
  {
    socketRoutedest = SoAd_SoConArr[SoConId].SocketRouteDestList[socketRoutedestCtn];

    upperRxPduId = SoAd_SocketRouteArr[socketRoutedestCtn].SoAdRxPduRef;

    SOAD_GET_UPPER_FNCTBL_BY_SOCKETROUTEDEST(socketRoutedest).UpperTpRxIndication(upperRxPduId, Result);

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

    /* SWS_SoAd_00741 */
    if(SOAD_GET_SOCON_GROUP(SoConId).SoAdSocketSoConModeChgNotification == SOAD_TRUE)
    {
      SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(pduroute).UpperSoConModeChg(SoConId, Mode);
    }
    pdurouteCtn++;
  }

  SoAd_DynSoConArr[SoConId].SoAdSoConState = Mode;
}

static void soad_SoConIpAssignChgNotifAllUppers(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State)
{
  uint32 pdurouteCtn = 0;
  uint32 pduroute = 0;

  while(pdurouteCtn < SoAd_SoConArr[SoConId].PduRouteSize)
  {
    pduroute = SoAd_SoConArr[SoConId].PduRouteList[pdurouteCtn];

    if(SOAD_GET_SOCON_GROUP(SoConId).SoAdSocketIpAddrAssignmentChgNotification == SOAD_TRUE)
    {
      SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(pduroute).LocalIpAddrAssignmentChg(SoConId, State);
    }
    pdurouteCtn++;
  }

  SoAd_DynSoConArr[SoConId].IpAddrState = State;
}

static void soad_HandleTpTxSession(PduIdType TxPduId)
{

  PduLengthType upperAvaiSize;
  PduInfoType pduInfo;
  BufReq_ReturnType upperBufferReqRet = BUFREQ_OK;

  Std_ReturnType ret = E_OK;

  SoAd_TxPdu_t *thisTxPdu = &SoAd_DynTxPdu[TxPduId];

  switch((thisTxPdu->TxSsState))
  {
    case SOAD_SS_START:
      /* <Up>_[SoAd][Tp]CopyTxData to ask size */
      pduInfo.SduDataPtr = NULL_PTR;
      pduInfo.SduLength = 0;

      upperBufferReqRet = SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperTpCopyTxData(
          TxPduId, &pduInfo, NULL_PTR, &upperAvaiSize);

      /* <Up>_[SoAd][Tp]CopyTxData */

      if(upperAvaiSize > 0)
      {
        pduInfo.SduLength = upperAvaiSize;
        pduInfo.SduDataPtr = &(thisTxPdu->PduData[0]);

        thisTxPdu->TxSsCopiedLength += pduInfo.SduLength;

        /* call Up>_[SoAd][Tp]CopyTxData */
        upperBufferReqRet = SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperTpCopyTxData(
            TxPduId, &pduInfo, NULL_PTR, &upperAvaiSize);

        if(upperBufferReqRet == BUFREQ_OK)
        {
          if(upperAvaiSize == 0)
          {
            //Copy done
            thisTxPdu->TxSsState = SOAD_SS_DONE;
          }else
          {
            //copy on going
            thisTxPdu->TxSsLastUpperAskedSize = upperAvaiSize;
            thisTxPdu->TxSsState = SOAD_SS_COPYING;
          }
        }
      }else
      {
        thisTxPdu->TxSsState = SOAD_SS_DONE;
        //TODO: consider confirm OK or NOT_OK
      }
      break;
    case SOAD_SS_COPYING:

      upperAvaiSize = thisTxPdu->TxSsLastUpperAskedSize;
      pduInfo.SduLength = upperAvaiSize;
      pduInfo.SduDataPtr = &(thisTxPdu->PduData[thisTxPdu->TxSsCopiedLength]);

      thisTxPdu->TxSsCopiedLength += pduInfo.SduLength;

      /* call Up>_[SoAd][Tp]CopyTxData, [SWS_SoAd_00554, SWS_SoAd_00555, SWS_SoAd_00556 */
      upperBufferReqRet = SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperTpCopyTxData(
          TxPduId, &pduInfo, NULL_PTR, &upperAvaiSize);

      if(upperBufferReqRet == BUFREQ_OK)
      {
        if(upperAvaiSize == 0)
        {
          //Copy done
          thisTxPdu->TxSsState = SOAD_SS_DONE;
        }else
        {
          //copy on going
          thisTxPdu->TxSsState = SOAD_SS_COPYING;
        }

        thisTxPdu->TxSsLastUpperAskedSize = upperAvaiSize;
      }

      break;
    case SOAD_SS_DONE:

      if(thisTxPdu->TxSsLastUpperAskedSize == 0)
      {
        pduInfo.SduDataPtr = &(thisTxPdu->PduData[0]);
        pduInfo.SduLength = thisTxPdu->TxSsCopiedLength;

        /* SWS_SoAd_00557, SWS_SoAd_00667, SWS_SoAd_00670, SWS_SoAd_00558 */
        ret = soad_SendSoCon(SOAD_GET_SOCONID_BY_TXPDU(TxPduId), &pduInfo);

        SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperTpTpTxConfirmation(TxPduId, ret);
      }else
      {
        /* SWS_SoAd_00640, SWS_SoAd_00652, SWS_SoAd_00651 */
        SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperTpTpTxConfirmation(TxPduId, E_NOT_OK);
      }

      thisTxPdu->TxSsState = SOAD_SS_STOP;
      thisTxPdu->TxSsCopiedLength = 0;
      thisTxPdu->TxSsLastUpperAskedSize = 0;
      break;
    case SOAD_SS_STOP:
      break;
    default:
      break;
  }

  if(upperBufferReqRet == BUFREQ_E_NOT_OK)
  {
    //TODO: handle BUFREQ_E_NOT_OK
  }
}

/* SWS_SoAd_00562 */
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
      soad_getSoConQueueFirstElement(&(thisSoCon->RxQueue), &soConBufferData);
      //Copy done -> UpperTpRxIndication
      /* SWS_SoAd_00641 */
      if(thisSoCon->RxSsCopiedLength != soConBufferData.length)
      {
        SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpRxIndication(socketRoute->SoAdRxPduRef, E_NOT_OK);
      }else
      {
        SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId).UpperTpRxIndication(socketRoute->SoAdRxPduRef, E_OK);
      }
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

void soad_InitSoConQueue(SoAd_SocketBufferQueue_t *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

boolean soad_EnqueueSoConData(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *buffer)
{
    if((queue->size) == SOAD_SOCON_QUEUE_DEPTH)
    {
        return FALSE; // Queue is full, unable to enqueue
    }

    queue->rear = (queue->rear + 1) % SOAD_SOCON_QUEUE_DEPTH;
    queue->Buffer[queue->rear] = *buffer;
    queue->size++;
    return TRUE;
}

boolean soad_getSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue, SoAd_SoConBuffer_t *dest)
{
    if((queue->size) == 0)
    {
        return FALSE; // Queue is empty, unable to get first element
    }

    *dest = queue->Buffer[queue->front];
    return TRUE;
}

boolean soad_RemoveSoConQueueFirstElement(SoAd_SocketBufferQueue_t *queue)
{
    if ((queue->size) == 0) {
        return FALSE; // Queue is empty, unable to remove first element
    }

    queue->front = (queue->front + 1) % SOAD_SOCON_QUEUE_DEPTH;
    queue->size--;
    return TRUE;
}

static boolean soad_isMulticastIpv4Addr(const char *ip_address)
{
    // Convert the IPv4 address to a 32-bit unsigned integer
  boolean ret = SOAD_FALSE;

  uint32_t ip_num = 0;
  int octet[4];

  if (sscanf(ip_address, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]) != 4)
  {
      return 0; // Invalid IP format
  }
  for (int i = 0; i < 4; i++)
  {
      if (octet[i] < 0 || octet[i] > 255) {
          ret = SOAD_FALSE; // Invalid octet value
      }
      ip_num = (ip_num << 8) | octet[i];
  }

  // Check if the IP address is within the multicast range
  uint32_t multicast_range_start = 0xE0000000; // 224.0.0.0
  uint32_t multicast_range_end = 0xEFFFFFFF;   // 239.255.255.255
  if (ip_num >= multicast_range_start && ip_num <= multicast_range_end)
  {
      ret = SOAD_TRUE; // IP address is multicast
  }

  return ret; // IP address is not multicast
}
/* ***************************** [ FUNCTIONS ] ****************************** */
