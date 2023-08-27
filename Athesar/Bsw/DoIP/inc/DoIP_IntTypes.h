#ifndef DOIP_INT_TYPES_H
#define DOIP_INT_TYPES_H

#include "Std_Types.h"
#include "SoAd.h"
#include "Ath_Types.h"
#include "DoIP_Cfg.h"
#include "DoIP_PubTypes.h"

#define DOIP_MODULE_ID ((uint16) 173)
#define DOIP_INSTANCE_ID ((uint16) 0)

#define DOIP_API_ID_ACTIVATIONLINESWITCH ((uint8) 0x0E)

#define DOIP_E_UNINIT                 ((uint8) 0x01)
#define DOIP_E_PARAM_POINTER          ((uint8) 0x01)
#define DOIP_E_INVALID_PDU_SDU_ID     ((uint8) 0x01)
#define DOIP_E_INVALID_PARAMETER      ((uint8) 0x01)
#define DOIP_E_INIT_FAILED            ((uint8) 0x01)

#define DOIP_MODULE_STATUS_DEFAULT 0x00
#define DOIP_MODULE_STATUS_INITIALIZED 0x01

#define DOIP_DEFAULT_NETMASK ((uint8) 0)

#define DOIP_IF_STATE_LINE_ACTIVATION_ACTIVE ((uint8) 0x01)

typedef struct
{
  const DoIP_EntityCfgType* CfgEntityPtr;
} DoIP_EntityDataType;

typedef struct
{
  const DoIP_InterfaceCfgType* CfgInterfacePtr;
  uint8 Status;
  boolean LineActivationState;
} DoIP_InterfaceDataType;

typedef struct
{
  const DoIP_ChannelCfgType* CfgChannelPtr;
} DoIP_ChannelDataType;

typedef struct
{
  const DoIP_RoutActCfgType* CfgRoutActPtr;
} DoIP_RoutActDataType;

typedef struct
{
  const DoIP_TesterCfgType* CfgTesterPtr;
} DoIP_TesterDataType;

typedef struct
{
  DoIP_ConCfgType* CfgTcpConPtr;
  Ath_TimeType InactivityTimer;
  Ath_TimeType AliveCheckTimer;
} DoIP_TcpConDataType;

typedef struct
{
  DoIP_ConCfgType* CfgUdpConPtr;
  Ath_TimeType VehIdDelayTimer;
} DoIP_UdpConDataType;

typedef struct
{
  DoIP_ConCfgType* CfgAnnConPtr;
  Ath_TimeType AnnTimer;
  boolean IsAnnTriggered;
  SoAd_SoConModeType SoConMode;
} DoIP_AnnConDataType;

typedef uint8 DoIP_ConType;
#define DOIP_CON_TYPE_TCP ((uint8) 0)
#define DOIP_CON_TYPE_UDP ((uint8) 1)
#define DOIP_CON_TYPE_ANN ((uint8) 2)

#endif
