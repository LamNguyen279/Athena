/*
 * SoAd.c
 *
 *  Created on: Jul 28, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd_Ram.h"
#include "SoAd.h"
#include "SoAd_Priv.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
void SoAd_Init(const SoAd_ConfigType *ConfigPtr) {

  SoAd_SoConIdType soConId = 0;
  uint32 soConGr = 0;

  SOAD_UNUSED(ConfigPtr);

  /* Initialize Dynamic SoCon Array */
  while(soConId < SoAd_SoConArrSize)
  {
    _SoAd_InitSocon(soConId);
    soConId++;
  }

  /* Initialize Dynamic SoConGr Array */
  while(soConGr < SoAd_SoConGrpArrSize)
  {
    _SoAd_InitSoConGroup(soConGr);
    soConGr++;
  }
}


void SoAd_MainFunction(void)
{
  for(PduIdType txPduId = 0; txPduId < SoAd_PduRouteArrSize; txPduId ++)
  {
    _SoAd_HandleTxData(txPduId);
  }

  for(SoAd_SoConIdType SoConId = 0; SoConId < SoAd_SoConArrSize; SoConId ++)
  {
    _SoAd_HandleSoConState(SoConId);

    _SoAd_HandleRxData(SoConId);
  }
}

void SoAd_LocalIpAddrAssignmentChg(TcpIp_LocalAddrIdType IpAddrId, TcpIp_IpAddrStateType State)
{

}

/* SWS_SoAd_00539 */
Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr)
{
  Std_ReturnType ret = E_OK;
  PduInfoType triggerPduInfo;

  if(TxPduId >= SoAd_PduRouteArrSize)
  {
    //TODO: raise DET
    ret = E_NOT_OK;
  }else
  {
    if(SoAd_PduRouteArr[TxPduId].SoAdTxUpperLayerType != SOAD_UPPER_IF)
    {
      ret = E_NOT_OK;
      //TODO raise DET
    }
  }

  if(PduInfoPtr == NULL_PTR)
  {
    /* SWS_SoAd_00731, SoAd shall use <Up>_[SoAd][If]TriggerTransmit>()  */
    if(SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperIfTriggerTransmit != NULL_PTR)
    {
      SOAD_GET_UPPER_FNCTBL_BY_PDUROUTE(TxPduId).UpperIfTriggerTransmit(TxPduId, &triggerPduInfo);
      ret = _SoAd_IfPduFanOut(TxPduId, PduInfoPtr);
    }
  }else
  {
    ret = _SoAd_IfPduFanOut(TxPduId, PduInfoPtr);
  }

  return ret;
}

/* SWS_SoAd_00551 */
Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr)
{
  Std_ReturnType ret = E_OK;

  SOAD_UNUSED(PduInfoPtr);

  if(TxPduId >= SoAd_PduRouteArrSize)
  {
    //TODO: raise DET
    ret = E_NOT_OK;
  }else
  {
    if(SoAd_PduRouteArr[TxPduId].SoAdTxUpperLayerType != SOAD_UPPER_TP)
    {
      ret = E_NOT_OK;
      //TODO raise DET
    }
  }

  if(ret == E_OK)
  {
    ret = _SoAd_RequestTpTxSs(TxPduId, PduInfoPtr);
  }

  return ret;
}

Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType *SoConIdPtr) {

  Std_ReturnType ret = E_OK;

  const SoAd_CfgPduRouteDest_t *SoAdPduRouteDest;

  const SoAd_CfgPduRoute_t *pduRoute;

  if(TxPduId >= SoAd_PduRouteArrSize)
  {
//    TODO: raise DET
    ret = E_NOT_OK;
  }

  if(SoConIdPtr == NULL_PTR)
  {
    //TODO: raise DET
    ret = E_NOT_OK;
  }

  if(ret == E_OK)
  {
    pduRoute = &SoAd_PduRouteArr[TxPduId];

    if(pduRoute->SoAdPduRouteDestCtn == 1)
    {
      SoAdPduRouteDest = &SoAd_PduRouteDestArr[pduRoute->SoAdPduRouteDestBase];

      if(SoAdPduRouteDest->SoAdTxSoConGrIdx == SOAD_INVALID_SOCON_GROUP)
      {
        if(SoAdPduRouteDest->SoAdTxSoConIdx != SOAD_INVALID_SOCON)
        {
          *SoConIdPtr = (SoAdPduRouteDest->SoAdTxSoConIdx);
        }else
        {
          //no SOCON in SoAdPduRouteDest -> invalid CONFIG
        }
      }else
      {
        /* SWS_SoAd_00724 */
        //call SoAd_GetSoConId in case fan out PDU
      }
    }else
    {
      /* SWS_SoAd_00724 */
      //call SoAd_GetSoConId in case fan out PDU
    }
  }

  return ret;
}

Std_ReturnType SoAd_GetLocalAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *LocalAddrPtr,
                                 uint8_t *NetmaskPtr, TcpIp_SockAddrType *DefaultRouterPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_SetRemoteAddr(SoAd_SoConIdType SoConId,
                                  const TcpIp_SockAddrType *RemoteAddrPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_GetRemoteAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *IpAddrPtr) {
  Std_ReturnType ret = E_NOT_OK;

  /* SWS_SoAd_00743 */

  return ret;
}

Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId)
{
  Std_ReturnType ret = E_OK;

  if(SoConId >= SoAd_SoConArrSize)
  {
    ret = E_NOT_OK;
    //TODO: raise DET
  }

  if(ret == E_OK)
  {
    if(!SOAD_CHECK_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN))
    {
      /* SWS_SoAd_00588 */
      SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);
    }
  }

  return ret;
}

Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort)
{
  Std_ReturnType ret = E_OK;

  if(SoConId >= SoAd_SoConArrSize)
  {
    ret = E_NOT_OK;
    //raise DET
  }

  if(ret == E_OK)
  {
    if(!SOAD_CHECK_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE))
    {
      /* SWS_SoAd_00588 */
      SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);
    }
  }

  return ret;
}
