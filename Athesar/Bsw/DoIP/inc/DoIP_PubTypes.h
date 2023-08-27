#ifndef DOIP_PUB_TYPES_H
#define DOIP_PUB_TYPES_H

#include "Std_Types.h"
#include "DoIP_Utils.h"
#include "SoAd.h"

#define DOIP_E_PENDING 0x10
#define DOIP_AUTOMATIC_ANNOUNCE ((uint8) 0)
#define DOIP_ONTRIGGER_ANNOUNCE ((uint8) 1)
#define DOIP_INVALID_CON_IDX ((uint16) 0xFFFF)

typedef struct
{
  DoIP_TimerType AliveCheckRespTimeout;
  DoIP_TimerType GeneralInactivityTime;
  DoIP_TimerType InitialInactivityTime;
  DoIP_TimerType InitialVehAnnTime;
  DoIP_TimerType VehicleAnnouncementInterval;
  uint16 ConStartIdx;
  uint16 TcpConStartIdx;
  uint16 UdpConStartIdx;
  uint16 AnnConStartIdx;
  uint16 NumTotalCon;
  uint8 NumTcpCon;
  uint8 NumUdpCon;
  uint8 NumAnnCon;
  uint8 VehicleAnnouncementCount;
  uint8 InterfaceAnnStart;
  uint8 MaxTesterConnections;
  boolean InterfaceActLineCtrl;
  boolean UseMacAddressForIdentification;
  boolean UseVehicleIdentificationSyncStatus;
} DoIP_InterfaceCfgType;

typedef struct
{
  PduIdType TxPduIdRef;
} DoIP_ChannelCfgType;

typedef struct
{
  boolean RequestAddressAssignment;
  boolean TcpConSecurityRequired;
  PduIdType SoAdRxPduIdRef;
  PduIdType SoAdTxPduIdRef;
  SoAd_SoConIdType SoConId;
} DoIP_ConCfgType;

typedef struct
{
  uint16 LA;
  uint8 Eid[6];
  uint8 Gid[6];
  boolean EidConfigured;
  boolean GidConfigured;
} DoIP_EntityCfgType;

typedef struct
{
  uint16 SA;
} DoIP_TesterCfgType;

typedef struct
{
  uint8 Number;
  boolean SecRequired;
} DoIP_RoutActCfgType;

typedef struct
{
  DoIP_EntityCfgType* EntityCfgPtr;
  DoIP_InterfaceCfgType** InterfaceCfgPtr;
  DoIP_ChannelCfgType** ChannelCfgPtr;
  DoIP_ConCfgType** TcpConCfgPtr;
  DoIP_ConCfgType** UdpConCfgPtr;
  DoIP_ConCfgType** AnnConCfgPtr;
  DoIP_TesterCfgType** TesterCfgPtr;
  DoIP_RoutActCfgType** RoutActCfgPtr;
} DoIP_ConfigType;

#endif
