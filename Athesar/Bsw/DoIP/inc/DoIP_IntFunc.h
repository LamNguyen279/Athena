#ifndef _DOIP_INT_FUNC_H_
#define _DOIP_INT_FUNC_H_

#include "DoIP_Globals.h"
#include "DoIP_Utils.h"

#define DOIP_IS_DEV_ERROR_DETECT_ENABLED (DOIP_DEV_ERROR_DETECT != STD_OFF)

#define DoIP_ReportDetErr(ApiId, ErrorId) Det_ReportError(DOIP_MODULE_ID, DOIP_INSTANCE_ID, ApiId, ErrorId)
#define DoIP_IsValidInterfaceId(InterfaceId) (InterfaceId < DOIP_NUM_INTERFACE)
#define DoIP_IsInterfaceActLineCtrl(InterfaceId) (DoIP_InterfaceData[InterfaceId].CfgInterfacePtr->InterfaceActLineCtrl != FALSE)
#define DoIP_IsNullPtr(ptr) (NULL_PTR == ptr)

#define DoIP_IsModuleInitialized() DoIP_CheckAnyFlagOn(DoIP_ModuleStatus, DOIP_MODULE_STATUS_INITIALIZED)
#define DoIP_SetModuleInitialized() DoIP_SetFlag(DoIP_ModuleStatus, DOIP_MODULE_STATUS_INITIALIZED)

#define DoIP_IsLineActivationActive(InterfaceId) (DoIP_InterfaceData[InterfaceId].LineActivationState != FALSE)
#define DoIP_GetLineActivationState(InterfaceId) DoIP_InterfaceData[InterfaceId].LineActivationState

#define DoIP_GetInterfaceCfg(InterfaceId) DoIP_InterfaceData[InterfaceId].CfgInterfacePtr
#define DoIP_GetInterfaceData(InterfaceId) &DoIP_InterfaceData[InterfaceId]


uint16 FindConBySoConId(SoAd_SoConIdType SoConId, DoIP_ConType* ConType);
void DoIP_SwitchLineActivationActive(uint8 InterfaceId);
void DoIP_SwitchLineActivationInactive(uint8 InterfaceId);

#endif