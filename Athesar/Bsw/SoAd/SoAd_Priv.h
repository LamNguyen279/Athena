/*
 * SoAd_Priv.h
 *
 *  Created on: Jul 30, 2023
 *      Author: lam nguyen
 *
               /\_/\   /\_/\
              ( o.o ) ( o.o )
               > ^ <   > ^ <
 *
 */

#ifndef SOAD_SOAD_PRIV_H_
#define SOAD_SOAD_PRIV_H_

/* ***************************** [ INCLUDES  ] ****************************** */
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "SoAd.h"
/* ***************************** [ MACROS    ] ****************************** */
#define SOAD_SOCON_BUFF_SIZE                      1500
#define SOAD_SOCON_QUEUE_DEPTH                    50
#define SOAD_SOCON_THREAD_STACK                   15000
//coding utilities
#define SOAD_GET_ARRAY_SIZE(array)  ( sizeof( (array) ) / sizeof( (array[0]) ) )

#define SOAD_BIG_ENDIAN             BIG_ENDIAN
#define SOAD_LITTLE_ENDIAN          LITTLE_ENDIAN

#ifdef SOAD_DEBUG
#include "stdio.h"
#define SOAD_LOG(SomeString) \
  printf("LINE: %d, FUNC: %s, LOG: ", __LINE__, __func__); \
  printf((SomeString)); \
  printf("\n"); \
  fflush(_REENT->_stdout)

#define SOAD_LOG_PAR(SomeString, SomeThing) \
  printf("LINE: %d, FUNC: %s, LOG: ", __LINE__, __func__); \
  printf((SomeString), (SomeThing)); \
  printf("\n"); \
  fflush(_REENT->_stdout)
#else
#define SOAD_CONSOLE_LOG(someString, SomeThing)
#endif

#define SOAD_TRUE                   STD_ON
#define SOAD_FALSE                  STD_OFF

#define SOAD_INVALID_PDU_HEADER     ((uint32)-1)
#define SOAD_INVALID_SOCON_GROUP    ((uint32)-1)
#define SOAD_INVALID_ROUT_GROUP     ((uint32)-1)
#define SOAD_INVALID_SOCON          ((SoAd_SoConIdType)-1)
#define SOAD_INVALID_NUMBER                -1

#define SOAD_RAISE_DEV_ERROR(ApiId, ErrorId)

#define SOAD_UNUSED(variable) (void)variable

#define SOAD_FOR_IN_RANGE(IdxVar, Range)\
    IdxVar = 0; \
    for (; (IdxVar) < (Range); (IdxVar)++)


#define SOAD_IPV4_ADD_SIZE                        15
//socket utilities
#define SOAD_SOCCON_REQMASK_NON    0
#define SOAD_SOCCON_REQMASK_OPEN   1
#define SOAD_SOCCON_REQMASK_CLOSE  2

#define SOAD_DYN_SOCON(SoConId)          (SoAd_DynSoConArr[(SoConId)])

#define SOAD_CHECK_SOCON_REQMASK(SoConId, mask) \
  (((SoAd_DynSoConArr[(SoConId)].RequestMask) & (mask)) == (mask))
#define SOAD_SET_SOCON_REQMASK(SoConId, mask) \
  (SoAd_DynSoConArr[(SoConId)].RequestMask = (mask))
#define SOAD_CLEAR_SOCON_REQMASK(SoConId, mask) \
  (SoAd_DynSoConArr[(SoConId)].RequestMask &= ~(mask))

#define SOAD_CHECK_SOCON_NEED_OPEN(SoConId) (SOAD_CHECK_SOCON_REQMASK((SoConId), SOAD_SOCCON_REQMASK_OPEN))
#define SOAD_CHECK_SOCON_NEED_CLOSE(SoConId) (SOAD_CHECK_SOCON_REQMASK((SoConId), SOAD_SOCCON_REQMASK_CLOSE))

#define SOAD_IS_SOCON_DATA_ONGOING(SoConId)       \
  (SoAd_DynSoConArr[(SoConId)].RxSsState != SOAD_SS_STOP) || \
  (SoAd_DynSoConArr[(SoConId)].TxSsState != SOAD_SS_STOP) || \
  (SoAd_DynSoConArr[(SoConId)].RxQueue.size != 0) || \
  (SoAd_DynSoConArr[(SoConId)].TxQueue.size != 0)

#define SOAD_GET_TCP_SOCON_BY_TXPDU(TxPduId)      (SoAd_PduRouteDestArr[SoAd_PduRouteArr[TxPduId].SoAdPduRouteDestBase].SoAdTxSoConIdx)

//socket group utilities
#define SOAD_GET_DYN_SOCON_GROUP(SoConId)         (SoAd_DynSoConGrArr[SOAD_GET_SOCON_GROUPID(SoConId)])

#define SOAD_GET_SOCON_GROUPID(SoConId)           (SoAd_SoConArr[(SoConId)].SoConGrIdx)
#define SOAD_GET_SOCON_GROUP(SoConId)             (SoAd_SoConGrArr[SoAd_SoConArr[(SoConId)].SoConGrIdx])
#define SOAD_GET_SOCON_PROTOCOL(SoConId)          (SOAD_GET_SOCON_GROUP((SoConId)).W32ProtocolType)
#define SOAD_IS_UDP_SOCON(SoConId)                (SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_UDP)
#define SOAD_IS_TCP_SOCON(SoConId)                (SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_TCP)
#define SOAD_IS_TCP_SERVER_SOCON(SoConId)         ( (SOAD_IS_TCP_SOCON((SoConId)) && SOAD_GET_SOCON_GROUP((SoConId)).SoAdSocketTcpInitiate) )

//W32 utilities
#define SOAD_W32_INVALID_SOCKET   INVALID_SOCKET
#define SOAD_W32_CHECK_ENV(ret) \
  WSADATA wsaDa; \
  int r = -1; \
  r = WSAStartup(MAKEWORD(2, 2), &wsaDa); \
  if (r != NO_ERROR) { \
      ret = E_NOT_OK; \
  }

//function table
#define SOAD_GET_TCP_SOCON_UPPER_FNCTB(SoConId)                       SOAD_GET_UPPER_FNCTBL_BY_SOCKETROUTEDEST(SoAd_SoConArr[(SoConId)].SocketRouteDestList[0])

#define SOAD_GET_TCP_SOCON_UPPER_LAYER(SoConId)                       SOAD_GET_TCP_SOCON_SOCKET_ROUTE(SoConId).SoAdRxUpperLayerType

#define SOAD_GET_TCP_SOCON_SOCKET_ROUTE(SoConId)                      SoAd_SocketRouteArr[SoAd_SoConArr[(SoConId)].SocketRouteDestList[0]]

#define SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(PduRoute)                   SoAd_UpperFunctionTable[SoAd_PduRouteArr[(PduRoute)].SoAdUpper]

#define SOAD_GET_UPPER_FNCTBL_BY_SOCKETROUTEDEST(SocketRouteDestId)   SoAd_UpperFunctionTable[SoAd_SocketRouteArr[(SocketRouteDestId)].SoAdUpper]
/* ***************************** [ TYPES     ] ****************************** */
typedef enum _SoAd_W32SocketType_t
{
  VTCPIP_SOCK_STREAM = 1,
  VTCPIP_SOCK_DGRAM = 2
} SoAd_W32SocketType_t;

typedef enum _SoAd_W32SocketAfType_t
{
  VTCPIP_AF_INET = 2,
  VTCPIP_AF_INET6 = 23
} SoAd_W32SocketAfType_t;

typedef enum _SoAd_W32SocketProtocolType_t
{
  VTCPIP_IPPROTO_TCP = 6,
  VTCPIP_IPPROTO_UDP = 17
} SoAd_W32SocketProtocolType_t;

typedef enum _SoAd_Upper_t
{
  SOAD_UPPER_INVALID = -1,
  SOAD_UPPER_DOIP
} SoAd_Upper_t;

typedef enum _SoAd_W32SocketState_t
{
  SOAD_W32SOCK_STATE_INVALID = 0,
  SOAD_W32SOCK_STATE_NEW,
  SOAD_W32SOCK_STATE_BIND,
  SOAD_W32SOCK_STATE_CONNECTING,
  SOAD_W32SOCK_STATE_CONNECTED,
  SOAD_W32SOCK_STATE_LISTENING,
  SOAD_W32SOCK_STATE_ACCEPTED
} SoAd_W32SocketState_t;

//configuration type
typedef struct _SoAd_UpperFncTable_t
{
  void (*UpperIfRxIndication)(PduIdType RxPduId, PduInfoType *PduInfoPtr);
  Std_ReturnType (*UpperIfTriggerTransmit)(PduIdType TxPduId, PduInfoType* PduInfoPtr);
  void (*UpperIfTxConfirmation)(PduIdType TxPduId, Std_ReturnType result);
  BufReq_ReturnType (*UpperTpStartOfReception)(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr);
  BufReq_ReturnType (*UpperTpCopyRxData)(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);
  void (*UpperTpRxIndication)(PduIdType id, Std_ReturnType result);
  BufReq_ReturnType (*UpperTpCopyTxData)(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);
  void (*UpperTpTpTxConfirmation)(PduIdType id, Std_ReturnType result);
  void (*UpperSoConModeChg) (SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode);
  void (*LocalIpAddrAssignmentChg) ( SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State);
} SoAd_CfgUpperFncTable_t;

typedef enum _SoAd_TxPduCollectionSemantics_t
{
  SOAD_COLLECT_QUEUE = 1,
  SOAD_COLLECT_LAST_IS_BEST
} SoAd_TxPduCollectionSemantics_t;

typedef enum _SoAd_TxUpperLayerType_t
{
  SOAD_UPPER_IF = 1,
  SOAD_UPPER_TP,
} SoAd_TxRxUpperLayerType_t;

/* SoAdRoutingGroup, TODO: generate to one array */
typedef struct _SoAdRoutingGroup_t
{
  //SoAdRoutingGroupId -> index of this array
  boolean IsEnabledAtInit; /* If set to true this routing group will be enabled after initializing the SoAd module */
  boolean TxTriggerable; /* referenced by this routing group can be triggered via SoAd_IfRoutingGroupTransmit (TRUE) or not (FALSE). */
} SoAd_CfgRoutingGroup_t;

/* SoAdPduRouteDest , TODO: generate to one array */
#define SOAD_TRIGGER_ALWAYS       1
#define SOAD_TRIGGER_NEVER        2

typedef struct _SoAd_PduRouteDest_t
{
  uint32 SoAdTxPduHeaderId;
  uint8 SoAdTxUdpTriggerMode;
  uint32 SoAdTxRoutingGroupBase;
  uint32 SoAdTxRoutingGroupCtn;
  uint32 SoAdTxSoConGrIdx;
  SoAd_SoConIdType SoAdTxSoConIdx;
} SoAd_CfgPduRouteDest_t;

/* PduRoute , TODO: generate to one array */
typedef struct _SoAd_CfgPduRoute_t
{
  SoAd_TxPduCollectionSemantics_t SoAdTxPduCollectionSemantics;
//  PduIdType SoAdTxPduId; /* index of this PduRoute */
  PduIdType SoAdTxPduRef; /* Upper TxPduId */
  SoAd_TxRxUpperLayerType_t SoAdTxUpperLayerType;
  SoAd_Upper_t SoAdUpper;
  uint32 SoAdPduRouteDestBase;
  uint32 SoAdPduRouteDestCtn;
} SoAd_CfgPduRoute_t;

/* SocketConnectionGroup, TODO: Generate to one array */
typedef struct _SoAd_CfgSoConGrp_t
{
  boolean SoAdPduHeaderEnable;
  boolean SoAdSocketAutomaticSoConSetup;
  boolean SoAdSocketFramePriority;
  boolean SoAdSocketIpAddrAssignmentChgNotification;
  uint32 SoAdSocketLocalPort;
  boolean SoAdSocketMsgAcceptanceFilterEnabled;
  boolean SoAdSocketSoConModeChgNotification;
  uint16 SoAdSocketTpRxBufferMin;
  boolean SoAdSocketTcpInitiate;
  //WIN32 SOCK Properties
  SoAd_W32SocketAfType_t W32AfType;
  SoAd_W32SocketType_t W32SocketType;
  SoAd_W32SocketProtocolType_t W32ProtocolType;
  char  W32LocalAddress[SOAD_IPV4_ADD_SIZE];
} SoAd_CfgSoConGrp_t;

/* SocketRoute */
typedef struct _SoAd_CfgSocketRoute_t
{
  uint32 SoAdRxPduHeaderId;
  PduIdType SoAdRxPduId;
  SoAd_TxRxUpperLayerType_t SoAdRxUpperLayerType;
  PduIdType SoAdRxPduRef; //To RxPduId of Upper
  uint32 SoAdRxRoutingGroupBase;
  uint32 SoAdRxRoutingGroupCtn;
  SoAd_Upper_t SoAdUpper;
} SoAd_CfgSocketRoute_t;

/* SoAd Socket Connection */
typedef struct _SoAd_CfgSoCon_t
{
  /* SoAdSocketId -> Index of array */
  sint8 RemoteIpAddress[SOAD_IPV4_ADD_SIZE]; /* remote address */
  uint32 RemotePort; /* remote port */
  uint32 SoConGrIdx; /* get local SoCon Group properties by this Id */
  const uint32 *PduRouteList; /* holds all PduRoute that refer to this SoCon */
  const uint32 PduRouteSize; /* PduRouteReferredListCtn */
  const uint32 *SocketRouteDestList;  /* holds all Socket RouteDest that refer to this SoCon */
  const uint32 SocketRouteDestListSize; /* SocketRouteDestReferredListCtn */
} SoAd_CfgSoCon_t;

//runtime type
typedef struct _SoAd_SocketBuffer_t
{
  uint8 data[SOAD_SOCON_BUFF_SIZE];
  uint32 length;
} SoAd_SoConBuffer_t;

typedef struct _SoAd_SocketBufferQueue_t {
  SoAd_SoConBuffer_t Buffer[SOAD_SOCON_QUEUE_DEPTH];
  uint32 front;
  uint32 rear;
  uint32 size;
} SoAd_SocketBufferQueue_t;

typedef enum _SoAd_TpSessionState_t
{
  SOAD_SS_STOP = 0,
  SOAD_SS_START,
  SOAD_SS_COPYING,
  SOAD_SS_DONE
} SoAd_TpSessionState_t;

typedef struct _SoAd_SoCon_t
{
  struct
  {
    HANDLE Hdl;
    DWORD Id;
  } W32Thread;
  SOCKET W32Sock;
  SoAd_W32SocketState_t W32SockState;
  /* for SoAd queue, copy session */
  uint32_t RxSsCopiedLength;
  uint32_t RxSsLastUpperAskedSize;
  SoAd_TpSessionState_t TxSsState;
  SoAd_TpSessionState_t RxSsState;
  SoAd_SocketBufferQueue_t TxQueue;
  SoAd_SocketBufferQueue_t RxQueue;
  /* dynamic remote address */
  char  RemoteAddress[SOAD_IPV4_ADD_SIZE];
  uint32 RemotePort;
  /* for AUTOSAR */
  uint32 RequestMask;
  SoAd_SoConIdType SoAdSoConId;
  SoAd_SoConModeType SoAdSoConState;
} SoAd_SoCon_t;

typedef struct _SoAdSoConGr_t
{
  //W32
  struct
  {
    HANDLE Hdl;
    DWORD Id;
  } W32Thread;
  SoAd_W32SocketState_t W32SockListenState;
  SOCKET W32SockListen;
} SoAd_SoConGr_t;

typedef struct _SoAd_TxPdu_t
{
  uint8 PduData[SOAD_SOCON_BUFF_SIZE];
  uint32_t TxSsCopiedLength;
  uint32_t TxSsLastUpperAskedSize;
  SoAd_TpSessionState_t TxSsState;
} SoAd_TxPdu_t;

/* ***************************** [ DECLARES  ] ****************************** */
extern void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId);
extern void _SoAd_HandleRxData(SoAd_SoConIdType SoConId);
extern void _SoAd_HandleTxData(PduIdType TxPduId);
extern void _SoAd_InitSocon(SoAd_SoConIdType SoConId);
extern void _SoAd_InitSoConGroup(uint32 SoConGr);
extern Std_ReturnType _SoAd_IfPduFanOut(PduIdType TxPduId, const PduInfoType *PduInfo);
extern Std_ReturnType _SoAd_RequestTpTxSs(PduIdType TxPduId, const PduInfoType *PduInfo);
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_PRIV_H_ */
