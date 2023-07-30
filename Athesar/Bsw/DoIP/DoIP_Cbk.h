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
    PduInfoType *info,
    PduLengthType TpSduLength,
    PduLengthType *bufferSizePtr);
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
#endif
