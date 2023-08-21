/*
 * DoIP_Cbk.h
 *
 *  Created on: Jul 29, 2023
 *      Author: ADMIN
 */
#ifndef DOIP_CBK_H
#define DOIP_CBK_H
/* ***************************** [ INCLUDES  ] ****************************** */
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "SoAd.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
extern void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  PduInfoType *PduInfoPtr);

extern void DoIP_SoConModeChg(
    SoAd_SoConIdType SoConId,
    SoAd_SoConModeType Mode);

extern BufReq_ReturnType DoIP_SoAdTpStartOfReception(
    PduIdType id,
    const PduInfoType *info,
    PduLengthType TpSduLength,
    PduLengthType *bufferSizePtr);

extern BufReq_ReturnType DoIP_SoAdTpCopyRxData(
  PduIdType id,
  const PduInfoType *info,
  PduLengthType *bufferSizePtr);

extern void DoIP_SoAdTpRxIndication(
    PduIdType id,
    Std_ReturnType result);

extern void DoIP_LocalIpAddrAssignmentChg(
    SoAd_SoConIdType SoConId,
    TcpIp_IpAddrStateType State);

extern void DoIP_SoAdIfTxConfirmation(
    PduIdType TxPduId,
    Std_ReturnType result);

extern BufReq_ReturnType DoIP_SoAdTpCopyTxData(
    const PduIdType id,
    const PduInfoType *info,
    const RetryInfoType *retry,
    PduLengthType *availableDataPtr);

extern void DoIP_SoAdTpTxConfirmation(
    PduIdType TxPduId,
    Std_ReturnType result);
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
#endif
