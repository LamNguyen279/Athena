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
static Std_ReturnType soad_validateSoconId(SoAd_SoConIdType SoconId);
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
  for(SoAd_SoConIdType SoConId = 0; SoConId < SoAd_SoConArrSize; SoConId ++)
  {
    _SoAd_HandleSoConState(SoConId);

    _SoAd_HandleSoConRxData(SoConId);
  }
}

void SoAd_LocalIpAddrAssignmentChg(TcpIp_LocalAddrIdType IpAddrId, TcpIp_IpAddrStateType State)
{

}

Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr)
{

  Std_ReturnType ret = E_OK;

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
    //TODO: raise DET
    ret = E_NOT_OK;
  }

  if(ret == E_OK)
  {
    ret = _SoAd_IfPduFanOut(TxPduId, PduInfoPtr);
  }

  return ret;
}

Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType *SoConIdPtr) {

  Std_ReturnType ret = E_OK;

  const SoAd_CfgPduRouteDest_t *SoAdPduRouteDest;

  const SoAd_CfgPduRoute_t *pduRoute;

  if(TxPduId >= SoAd_PduRouteArrSize)
  {
//    TODO: SOAD_RAISE_DEV_ERROR();
    ret = E_NOT_OK;
  }

  if(SoConIdPtr == NULL_PTR)
  {
    //TODO: raise dev
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
        //call SoAd_GetSoConId in case fan out PDU
      }
    }else
    {
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

  return ret;
}

Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId) {
  Std_ReturnType ret = E_OK;

  ret = soad_validateSoconId(SoConId);

  if(ret == E_OK)
  {
    if(!SOAD_CHECK_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN))
    {
      SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);
    }
  }else
  {
    //TODO: raise DET
  }

  return ret;
}

Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort) {
  Std_ReturnType ret = E_NOT_OK;

  ret = soad_validateSoconId(SoConId);

  if(ret == E_OK)
  {
    if(!SOAD_CHECK_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE))
    {
      SOAD_SET_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_CLOSE);
      SOAD_CLEAR_SOCON_REQMASK(SoConId, SOAD_SOCCON_REQMASK_OPEN);
    }
  }else
  {
    //TODO: raise DET
  }

  return ret;
}


static Std_ReturnType soad_validateSoconId(SoAd_SoConIdType SoconId)
{
  Std_ReturnType ret = E_OK;

  if(SoconId >= SoAd_SoConArrSize)
  {
    ret = E_NOT_OK;
  }

  return ret;
}
