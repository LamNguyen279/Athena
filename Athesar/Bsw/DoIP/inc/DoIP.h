/*
 * DoIP_Cbk.h
 *
 *  Created on: Jul 29, 2023
 *      Author: ADMIN
 */
#ifndef _DOIP_H_
#define _DOIP_H_
/* ***************************** [ INCLUDES  ] ****************************** */
#include "Std_Types.h"
#include "ComStack_Types.h"
#include "DoIP_PubTypes.h"
#include "SoAd.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
/* @SWS_DoIP_00277 */
Std_ReturnType DoIP_TpTransmit(
  PduIdType TxPduId,
  const PduInfoType* PduInfoPtr);

/* @SWS_DoIP_00278 */
Std_ReturnType DoIP_TpCancelTransmit(
  PduIdType TxPduId);

/* @SWS_DoIP_00026 */
void DoIP_Init(
  const DoIP_ConfigType* DoIPConfigPtr);

/* @SWS_DoIP_00027 */
void DoIP_GetVersionInfo(
  Std_VersionInfoType* versioninfo);

/* @SWS_DoIP_91000 */
void DoIP_ActivationLineSwitch(
  uint8 InterfaceId,
  boolean* Active);

/* @SWS_DoIP_91002 */
void DoIP_TriggerVehicleAnnouncement(
  uint8 InterfaceId);

/****** Callback functions ********/

/* @SWS_DoIP_00031 */
BufReq_ReturnType DoIP_SoAdTpCopyTxData(
  PduIdType id,
  const PduInfoType* info,
  const RetryInfoType* retry,
  PduLengthType* availableDataPtr);

/* @SWS_DoIP_00032 */
void DoIP_SoAdTpTxConfirmation(
  PduIdType id,
  Std_ReturnType result);

/* @SWS_DoIP_00033 */
BufReq_ReturnType DoIP_SoAdTpCopyRxData(
  PduIdType id,
  const PduInfoType* info,
  PduLengthType* bufferSizePtr);

/* @SWS_DoIP_00037 */
BufReq_ReturnType DoIP_SoAdTpStartOfReception (
  PduIdType id,
  const PduInfoType* info,
  PduLengthType TpSduLength,
  PduLengthType* bufferSizePtr);

/* @SWS_DoIP_00038 */
void DoIP_SoAdTpRxIndication(
  PduIdType id,
  Std_ReturnType result);

/* @SWS_DoIP_00244 */
void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  const PduInfoType* PduInfoPtr);

/* @SWS_DoIP_00245 */
void DoIP_SoAdIfTxConfirmation(
  PduIdType TxPduId,
  Std_ReturnType result);

/* @SWS_DoIP_00039 */
void DoIP_SoConModeChg(
  SoAd_SoConIdType SoConId,
  SoAd_SoConModeType Mode);

/* @SWS_DoIP_00040 */
void DoIP_LocalIpAddrAssignmentChg(
SoAd_SoConIdType SoConId,
TcpIp_IpAddrStateType State);

/****** Scheduler functions ********/

/* @SWS_DoIP_00041 */
void DoIP_MainFunction();

/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
#endif
