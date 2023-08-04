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

#define SOAD_GET_ARRAY_SIZE(array)  ( sizeof( (array) ) / sizeof( (array[0]) ) )

#define SOAD_BIG_ENDIAN             BIG_ENDIAN
#define SOAD_LITTLE_ENDIAN          LITTLE_ENDIAN

#define SOAD_TRUE                   STD_ON
#define SOAD_FALSE                  STD_OFF

#define SOAD_INVALID_PDU_HEADER     ((uint32)-1)
#define SOAD_INVALID_SOCON_GROUP    ((uint32)-1)
#define SOAD_INVALID_ROUT_GROUP     ((uint32)-1)
#define SOAD_INVALID_SOCON          ((SoAd_SoConIdType)-1)

#define SOAD_RAISE_DEV_ERROR(ApiId, ErrorId)

//socket utilities
#define _SOAD_SOCCON_REQMASK_OPEN   1
#define _SOAD_SOCCON_REQMASK_CLOSE  2

#define _SOAD_GET_SOCON_HDL(SoConId)          (SoAd_DynSoConArr[(SoConId)])

#define _SOAD_CHECK_SOCON_REQMASK(SoConId, mask) \
  (((SoAd_DynSoConArr[(SoConId)].RequestMask) & (mask)) == (mask))
#define _SOAD_SET_SOCON_REQMASK(SoConId, mask) \
  (SoAd_DynSoConArr[(SoConId)].RequestMask = (mask))
#define _SOAD_CLEAR_SOCON_REQMASK(SoConId, mask) \
  (SoAd_DynSoConArr[(SoConId)].RequestMask &= ~(mask))

#define _SOAD_CHECK_SOCCON_NEED_OPEN(SoConId) (_SOAD_CHECK_SOCON_REQMASK((SoConId), _SOAD_SOCCON_REQMASK_OPEN))

#define _SOAD_GET_SOCON_FNCTBL(SoConId)       (SoAd_UpperFunctionTable[_SOAD_GET_SOCON_HDL((SoConId)).Upper])

//socket group utilities
#define _SOAD_GET_SOCON_GROUP(SoConId)        ( (SoAdConGroupHandler_t *)(SoAd_DynSoConArr[(SoConId)].GrAssigned) )
#define _SOAD_GET_SOCON_PROTOCOL(SoConId)     (_SOAD_GET_SOCON_GROUP((SoConId))->ProtocolType)
#define _SOAD_IS_UDP_SOCON(SoConId)           (_SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_UDP)
#define _SOAD_IS_TCP_SOCON(SoConId)           (_SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_TCP)
#define _SOAD_IS_TCP_SERVER_SOCON(SoConId)    ( (_SOAD_IS_TCP_SOCON((SoConId)) && _SOAD_GET_SOCON_GROUP((SoConId))->IsServer) )

//Upper call utilities


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
  SOAD_SOCK_STATE_INVALID = 0,
  SOAD_SOCK_STATE_NEW,
  SOAD_SOCK_STATE_BIND,
  SOAD_SOCK_STATE_CONNECTING,
  SOAD_SOCK_STATE_CONNECTED,
  SOAD_SOCK_STATE_LISTENING,
  SOAD_SOCK_STATE_ACCEPTED
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
  uint32 SoAdTxRoutingGroupIdx;
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
  uint32 SoAdPduRouteDestBase;
  uint32 SoAdPduRouteDestCtn;
} SoAd_CfgPduRoute_t;

/* SocketConnectionGroup, TODO: Generate to one array */
typedef struct _SoAd_CfgSoConGrp_t
{
  SoAd_Upper_t SoAdUpperLayer;
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

typedef struct _SoAd_CfgSoConGrPar_t
{
  boolean IsServer;
  SoAd_W32SocketAfType_t AfType;
  SoAd_W32SocketType_t SocketType;
  SoAd_W32SocketProtocolType_t Protocol;
  char  LocalAddress[SOAD_IPV4_ADD_SIZE];
  uint32  LocalPort;
  boolean SoAdSocketIpAddrAssignmentChgNotification;
  boolean SoAdSocketSoConModeChgNotification;
} SoAd_CfgSoConGrPar_t;

/* SocketRoute */
typedef struct _SoAd_CfgSocketRoute_t
{
  uint32 SoAdRxPduHeaderId;
  PduIdType SoAdRxPduId;
  SoAd_TxRxUpperLayerType_t SoAdRxUpperLayerType;
  PduIdType SoAdRxPduRef; //To RxPduId of Upper
  uint32 SoAdRxRoutingGroupBase;
  uint32 SoAdRxRoutingGroupCtn;
} SoAd_CfgSocketRoute_t;

/* SoAd Socket Connection */
typedef struct _SoAd_SoCon_t
{
  /* SoAdSocketId -> Index of array */
  sint8 SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE]; /* remote address */
  uint32 SoAdSocketRemotePort; /* remote port */
  uint32 SoConGrIdx; /* get local SoCon Group properties by this Id */
  const uint32 *PduRouteDestList; /* holds all PduRouteDest that refer to this SoCon */
  const uint32 PduRouteDestListSize; /* PduRouteDestReferredListCtn */
  const uint32 *SocketRouteDestList;  /* holds all Socket RouteDest that refer to this SoCon */
  const uint32 SocketRouteDestListSize; /* SocketRouteDestReferredListCtn */
} SoAd_CfgSoCon_t;

//runtime type
typedef struct _SoAdSock_t
{
  struct
  {
    HANDLE Hdl;
    DWORD Id;
  } W32Thread;
  SOCKET W32Sock;
  SoAd_W32SocketState_t W32SockState;
  char *RxBuff;
  char *TxBuff;
  uint32_t RxLength;
  void *GrAssigned;
  char  RemoteAddress[SOAD_IPV4_ADD_SIZE];
  uint32 RemotePort;
  SoAd_Upper_t Upper;
  //AUTOSAR
  uint32 RequestMask;
  SoAd_SoConIdType SoAdSoConId;
  SoAd_SoConModeType SoAdSoConState;
  PduIdType TxPduId;
  PduIdType UpperRxPduId;
  PduIdType UpperConfTxPduId;
} SoAdSock_t;

typedef struct _SoAdConGroup_t
{
  //static socket Group properties
  boolean IsServer;
  char  LocalAddress[SOAD_IPV4_ADD_SIZE];
  int  AddressLength;
  SoAd_W32SocketAfType_t AfType;
  SoAd_W32SocketType_t SocketType;
  SoAd_W32SocketProtocolType_t ProtocolType;
  uint32 LocalPort;
  boolean SoAdSocketIpAddrAssignmentChgNotification;
  boolean SoAdSocketSoConModeChgNotification;
  // static socket Group protocol properties
  //W32 socket
  SoAd_W32SocketState_t W32SockListenState;
  SOCKET W32SockListen;
} SoAdConGroupHandler_t;

/* ***************************** [ DECLARES  ] ****************************** */
void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId);
void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId);
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_PRIV_H_ */
