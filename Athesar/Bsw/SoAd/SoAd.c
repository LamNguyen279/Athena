/*
 * SoAd.c
 *
 *  Created on: Jul 28, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd.h"
#include "SoAd_Priv.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
void SoAd_Init(const SoAd_ConfigType *ConfigPtr) {

}


void SoAd_MainFunction(void)
{
  for(SoAd_SoConIdType SoConId = 0; SoConId < _SoAd_DynSoConArrCtn; SoConId ++)
  {
    _SoAd_HandleSoConState(SoConId);

    _SoAd_HandleSoConRxData(SoConId);
  }
}

Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr) {
  Std_ReturnType ret = E_NOT_OK;

  return ret;
}

Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType *SoConIdPtr) {

  Std_ReturnType ret = E_NOT_OK;

  for(int idx = 0; idx < _SoAd_DynSoConArrCtn; idx++)
  {
    if(_SoAd_DynSoConArr[idx].TxPduId == TxPduId)
    {
      *SoConIdPtr = _SoAd_DynSoConArr[idx].SoAdSoConId;
      break;
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
  Std_ReturnType ret = E_OK;;

  if(!_SOAD_CHECK_SOCON_REQMASK(SoConId, _SOAD_SOCCON_REQMASK_OPEN))
  {
    _SOAD_SET_SOCON_REQMASK(SoConId, _SOAD_SOCCON_REQMASK_OPEN);
  }

  return ret;
}

Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort) {
  Std_ReturnType ret = E_NOT_OK;


  return ret;
}
