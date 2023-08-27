#ifndef _DOIP_GLOBALS_H_
#define _DOIP_GLOBALS_H_

#include "Std_Types.h"
#include "DoIP_IntTypes.h"
#include "DoIP_Cfg.h"

extern uint8 DoIP_ModuleStatus;

extern DoIP_EntityDataType DoIP_EntityData;
extern DoIP_InterfaceDataType DoIP_InterfaceData[DOIP_NUM_INTERFACE];
extern DoIP_ChannelDataType DoIP_ChannelData[DOIP_NUM_CHAN];
extern DoIP_TcpConDataType DoIP_TcpConData[DOIP_NUM_TCP_CON];
extern DoIP_UdpConDataType DoIP_UdpConData[DOIP_NUM_UDP_CON];
extern DoIP_AnnConDataType DoIP_AnnConData[DOIP_NUM_ANN_CON];
extern DoIP_RoutActDataType DoIP_RoutActData[DOIP_NUM_ROUT_ACT];
extern DoIP_TesterDataType DoIP_TesterData[DOIP_NUM_TESTER];

#endif