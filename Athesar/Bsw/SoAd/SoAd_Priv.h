/*
 * SoAd_Priv.h
 *
 *  Created on: Jul 30, 2023
 *      Author: ADMIN
 */

#ifndef SOAD_SOAD_PRIV_H_
#define SOAD_SOAD_PRIV_H_

/* ***************************** [ INCLUDES  ] ****************************** */
/* ***************************** [ MACROS    ] ****************************** */
//check socket mask
#define _SOAD_SOCCON_REQMASK_OPEN   1
#define _SOAD_SOCCON_REQMASK_CLOSE  2
#define _SOAD_CHECK_SOCON_REQMASK(SoConId, mask) \
  (((_SoAd_DynSoConArr[(SoConId)].RequestMask) & (mask)) == (mask))
#define _SOAD_SET_SOCON_REQMASK(SoConId, mask) \
  (_SoAd_DynSoConArr[(SoConId)].RequestMask = (mask))
#define _SOAD_CLEAR_SOCON_REQMASK(SoConId, mask) \
  (_SoAd_DynSoConArr[(SoConId)].RequestMask &= ~(mask))
#define _SOAD_IS_SOCON_NEED_OPEN(SoConId) _SOAD_CHECK_SOCON_REQMASK((SoConId), _SOAD_SOCCON_REQMASK_OPEN)


/* ***************************** [ TYPES     ] ****************************** */
typedef struct _SoAd_UpperFncTable_t
{
  void (*UpperIfRxIndication)(PduIdType RxPduId, PduInfoType *PduInfoPtr);
} SoAd_UpperFncTable_t;

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
Std_ReturnType _SoAd_OpenUdpSocket(SoAd_SoConIdType SoConId);
void _SoAd_HandleSoConState(SoAd_SoConIdType SoConId);
void _SoAd_HandleSoConRxData(SoAd_SoConIdType SoConId);
void _SoAd_SocketRoutine(SoAd_SoConIdType *SoConId);

/* ***************************** [ DATAS     ] ****************************** */
uint32_t _SoAd_DynSoConArrCtn;
SoAdSock_t _SoAd_DynSoConArr[];

SoAd_UpperFncTable_t _SoAd_UpperFunctionTable[];
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_PRIV_H_ */
