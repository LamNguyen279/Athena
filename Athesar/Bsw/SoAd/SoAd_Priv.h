/*
 * SoAd_Priv.h
 *
 *  Created on: Jul 30, 2023
 *      Author: ADMIN
 */

#ifndef SOAD_SOAD_PRIV_H_
#define SOAD_SOAD_PRIV_H_

/* ***************************** [ INCLUDES  ] ****************************** */
#include "Std_Types.h"
/* ***************************** [ MACROS    ] ****************************** */
//socket utilities
#define _SOAD_SOCCON_REQMASK_OPEN   1
#define _SOAD_SOCCON_REQMASK_CLOSE  2

#define _SOAD_GET_SOCON_HDL(SoConId)          (_SoAd_DynSoConArr[(SoConId)])

#define _SOAD_CHECK_SOCON_REQMASK(SoConId, mask) \
  (((_SoAd_DynSoConArr[(SoConId)].RequestMask) & (mask)) == (mask))
#define _SOAD_SET_SOCON_REQMASK(SoConId, mask) \
  (_SoAd_DynSoConArr[(SoConId)].RequestMask = (mask))
#define _SOAD_CLEAR_SOCON_REQMASK(SoConId, mask) \
  (_SoAd_DynSoConArr[(SoConId)].RequestMask &= ~(mask))

#define _SOAD_CHECK_SOCCON_NEED_OPEN(SoConId) (_SOAD_CHECK_SOCON_REQMASK((SoConId), _SOAD_SOCCON_REQMASK_OPEN))

#define _SOAD_GET_SOCON_FNCTBL(SoConId)       (_SoAd_UpperFunctionTable[_SOAD_GET_SOCON_HDL((SoConId)).Upper])

//socket group utilities
#define _SOAD_GET_SOCON_GROUP(SoConId)        ((SoAdConGroupHandler_t *)(_SoAd_DynSoConArr[(SoConId)].GrAssigned))
#define _SOAD_GET_SOCON_PROTOCOL(SoConId)     (_SOAD_GET_SOCON_GROUP((SoConId))->ProtocolType)
#define _SOAD_IS_UDP_SOCON(SoConId)           (_SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_UDP)
#define _SOAD_IS_TCP_SOCON(SoConId)           (_SOAD_GET_SOCON_PROTOCOL(SoConId) == VTCPIP_IPPROTO_TCP)

//Upper call utilities
#define _SOAD_GET_UPPER_FNCTBL(SoConId)       ()

/* ***************************** [ TYPES     ] ****************************** */
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
} SoAd_UpperFncTable_t;

typedef enum _SoAd_W32SocketState_t
{
  _SOAD_SOCK_STATE_INVALID = 0,
  _SOAD_SOCK_STATE_NEW,
  _SOAD_SOCK_STATE_BIND,
  _SOAD_SOCK_STATE_CONNECTED,
  _SOAD_SOCK_STATE_LISTENING,
  _SOAD_SOCK_STATE_ACCEPTED
} SoAd_W32SocketState_t;

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
  SoAdUpper_t Upper;
  //AUTOSAR
  uint32 RequestMask;
  SoAd_SoConIdType SoAdSoConId;
  SoAd_SoConModeType SoAdSoConState;
  PduIdType TxPduId;
  PduIdType UpperRxPduId;
  PduIdType UpperConfTxPduId;
} SoAdSock_t;
/* ***************************** [ DECLARES  ] ****************************** */
void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId);
void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId);
/* ***************************** [ DATAS     ] ****************************** */
uint32_t _SoAd_DynSoConArrCtn;
SoAdSock_t _SoAd_DynSoConArr[SOAD_CFG_NUM_SOAD_SOCKS];

SoAd_UpperFncTable_t _SoAd_UpperFunctionTable[];
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_PRIV_H_ */
