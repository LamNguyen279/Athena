#include "Std_Types.h"
#include "ComStack_Types.h"
#include "SoAd.h"
#include "Det.h"

#include "DoIP.h"

#include "Ath_Utils.h"
#include "DoIP_IntTypes.h"
#include "DoIP_Cfg.h"
#include "DoIP_PbCfg.h"
#include "DoIP_IntFunc.h"

/* @SWS_DoIP_00277 */
Std_ReturnType DoIP_TpTransmit(
  PduIdType TxPduId,
  const PduInfoType* PduInfoPtr);

/* @SWS_DoIP_00278 */
Std_ReturnType DoIP_TpCancelTransmit(
  PduIdType TxPduId);

/* @SWS_DoIP_00026 */
void DoIP_Init(
  const DoIP_ConfigType* DoIPConfigPtr)
{
  uint16 i;

  if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->EntityCfgPtr != NULL_PTR))
  {
    DoIP_EntityData.CfgEntityPtr = DoIPConfigPtr->EntityCfgPtr;
  }

  for (i = 0; i < DOIP_NUM_INTERFACE; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->InterfaceCfgPtr[i] != NULL_PTR))
    {
      DoIP_InterfaceData[i].CfgInterfacePtr = DoIPConfigPtr->InterfaceCfgPtr[i];
    }
    else
    {
      DoIP_InterfaceData[i].CfgInterfacePtr = &DoIP_InterfacePreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_CHAN; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->ChannelCfgPtr[i] != NULL_PTR))
    {
      DoIP_ChannelData[i].CfgChannelPtr = DoIPConfigPtr->ChannelCfgPtr[i];
    }
    else
    {
      DoIP_ChannelData[i].CfgChannelPtr = &DoIP_ChannelPreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_TESTER; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->ChannelCfgPtr[i] != NULL_PTR))
    {
      DoIP_TesterData[i].CfgTesterPtr = DoIPConfigPtr->TesterCfgPtr[i];
    }
    else
    {
      DoIP_TesterData[i].CfgTesterPtr = &DoIP_TesterPreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_ROUT_ACT; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->RoutActCfgPtr[i] != NULL_PTR))
    {
      DoIP_RoutActData[i].CfgRoutActPtr = DoIPConfigPtr->RoutActCfgPtr[i];
    }
    else
    {
      DoIP_RoutActData[i].CfgRoutActPtr = &DoIP_RoutActPreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_TCP_CON; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->TcpConCfgPtr[i] != NULL_PTR))
    {
      DoIP_TcpConData[i].CfgTcpConPtr = DoIPConfigPtr->TcpConCfgPtr[i];
    }
    else
    {
      DoIP_TcpConData[i].CfgTcpConPtr = &DoIP_TcpConPreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_UDP_CON; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->UdpConCfgPtr[i] != NULL_PTR))
    {
      DoIP_UdpConData[i].CfgUdpConPtr = DoIPConfigPtr->UdpConCfgPtr[i];
    }
    else
    {
      DoIP_UdpConData[i].CfgUdpConPtr = &DoIP_UdpConPreCfg[i];
    }
  }

  for (i = 0; i < DOIP_NUM_ANN_CON; i++)
  {
    if((DoIPConfigPtr != NULL_PTR) && (DoIPConfigPtr->AnnConCfgPtr[i] != NULL_PTR))
    {
      DoIP_AnnConData[i].CfgAnnConPtr = DoIPConfigPtr->AnnConCfgPtr[i];
    }
    else
    {
      DoIP_AnnConData[i].CfgAnnConPtr = &DoIP_AnnConPreCfg[i];
    }
  }
  
  DoIP_SetModuleInitialized();
}

/* @SWS_DoIP_00027 */
void DoIP_GetVersionInfo(
  Std_VersionInfoType* versioninfo)
{
  versioninfo->vendorId = ATHENA_VENDOR_ID;
  versioninfo->moduleID = DOIP_MODULE_ID;
  versioninfo->sw_major_version = DOIP_SW_MAJOR_VERSION;
  versioninfo->sw_minor_version = DOIP_SW_MINOR_VERSION;
  versioninfo->sw_patch_version = DOIP_SW_PATCH_VERSION;
}

/* @SWS_DoIP_91000 */
void DoIP_ActivationLineSwitch(
  uint8 InterfaceId,
  boolean* Active)
{
  #if(DOIP_IS_DEV_ERROR_DETECT_ENABLED)
  if(!DoIP_IsModuleInitialized())
  {
    DoIP_ReportDetErr(DOIP_API_ID_ACTIVATIONLINESWITCH, DOIP_E_UNINIT);
  }
  else if(!DoIP_IsValidInterfaceId(InterfaceId) ||
          !DoIP_IsInterfaceActLineCtrl(InterfaceId))
  {
    DoIP_ReportDetErr(DOIP_API_ID_ACTIVATIONLINESWITCH, DOIP_E_INVALID_PARAMETER);
  }
  else if(DoIP_IsNullPtr(Active))
  {
    DoIP_ReportDetErr(DOIP_API_ID_ACTIVATIONLINESWITCH, DOIP_E_PARAM_POINTER);
  }
  else
  #endif
  {
    if(*Active == DoIP_GetLineActivationState(InterfaceId))
    {
      DoIP_GetLineActivationState(InterfaceId) = *Active;

      if(*Active == FALSE)
      {
        DoIP_SwitchLineActivationActive(InterfaceId);
      }
      else
      {
        DoIP_SwitchLineActivationInactive(InterfaceId);
      }
    }
  }
}

/* @SWS_DoIP_91002 */
void DoIP_TriggerVehicleAnnouncement(
  uint8 InterfaceId)
{
  uint16 conIdx;
  uint16 conEndIdx;
  DoIP_InterfaceCfgType* CfgInterfacePtr;

  CfgInterfacePtr = DoIP_GetInterfaceCfg(InterfaceId);
  conIdx = CfgInterfacePtr->AnnConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumAnnCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(SOAD_SOCON_ONLINE == DoIP_AnnConData[conIdx].SoConMode)
    {
      DoIP_StartTimer(DoIP_AnnConData[conIdx].AnnTimer, CfgInterfacePtr->InitialVehAnnTime);
    }
    DoIP_AnnConData[conIdx].IsAnnTriggered = TRUE;
  }
}

/****** Callback functions ********/

/* @SWS_DoIP_00031 */
BufReq_ReturnType DoIP_SoAdTpCopyTxData(
  PduIdType id,
  const PduInfoType* info,
  const RetryInfoType* retry,
  PduLengthType* availableDataPtr)
{
  BufReq_ReturnType retVal;

  retVal = E_OK;

  return retVal;
}

/* @SWS_DoIP_00032 */
void DoIP_SoAdTpTxConfirmation(
  PduIdType id,
  Std_ReturnType result)
{}

/* @SWS_DoIP_00033 */
BufReq_ReturnType DoIP_SoAdTpCopyRxData(
  PduIdType id,
  const PduInfoType* info,
  PduLengthType* bufferSizePtr)
{
  BufReq_ReturnType retVal;

  retVal = E_OK;

  return retVal;
}

/* @SWS_DoIP_00037 */
BufReq_ReturnType DoIP_SoAdTpStartOfReception (
  PduIdType id,
  const PduInfoType* info,
  PduLengthType TpSduLength,
  PduLengthType* bufferSizePtr)
{
  BufReq_ReturnType retVal;

  retVal = E_OK;

  return retVal;
}

/* @SWS_DoIP_00038 */
void DoIP_SoAdTpRxIndication(
  PduIdType id,
  Std_ReturnType result)
{}

/* @SWS_DoIP_00244 */
void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  const PduInfoType* PduInfoPtr)
{}

/* @SWS_DoIP_00245 */
void DoIP_SoAdIfTxConfirmation(
  PduIdType TxPduId,
  Std_ReturnType result)
{}

/* @SWS_DoIP_00039 */
void DoIP_SoConModeChg(
  SoAd_SoConIdType SoConId,
  SoAd_SoConModeType Mode)
{
  
}

/* @SWS_DoIP_00040 */
void DoIP_LocalIpAddrAssignmentChg(
  SoAd_SoConIdType SoConId,
  TcpIp_IpAddrStateType State)
{}

/****** Scheduler functions ********/

/* @SWS_DoIP_00041 */
void DoIP_MainFunction()
{

}