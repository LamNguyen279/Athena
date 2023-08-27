#ifndef _DOIP_CFG_H_
#define _DOIP_CFG_H_

#include "Std_Types.h"
#include "DoIP_PubTypes.h"

#define DOIP_SW_MAJOR_VERSION ((uint8) 0)
#define DOIP_SW_MINOR_VERSION ((uint8) 1)
#define DOIP_SW_PATCH_VERSION ((uint8) 0)

#define DOIP_NUM_INTERFACE ((uint8) 1)
#define DOIP_NUM_CHAN ((uint8) 3)
#define DOIP_NUM_CON_TOTAL ((uint8) 5)
#define DOIP_NUM_TCP_CON ((uint8) 3)
#define DOIP_NUM_UDP_CON ((uint8) 1)
#define DOIP_NUM_ANN_CON ((uint8) 1)
#define DOIP_NUM_ROUT_ACT ((uint8) 3)
#define DOIP_NUM_TESTER ((uint8) 3)

#define DOIP_DEV_ERROR_DETECT   STD_ON

extern DoIP_EntityCfgType DoIP_EntityPreCfg;
extern DoIP_InterfaceCfgType* DoIP_InterfacePreCfg;
extern DoIP_ChannelCfgType* DoIP_ChannelPreCfg[DOIP_NUM_CHAN];
extern DoIP_ConCfgType* DoIP_TcpConPreCfg[DOIP_NUM_TCP_CON];
extern DoIP_ConCfgType* DoIP_UdpConPreCfg[DOIP_NUM_UDP_CON];
extern DoIP_ConCfgType* DoIP_AnnConPreCfg[DOIP_NUM_ANN_CON];
extern DoIP_RoutActCfgType* DoIP_RoutActPreCfg[DOIP_NUM_ROUT_ACT];
extern DoIP_TesterCfgType* DoIP_TesterPreCfg[DOIP_NUM_TESTER];

#endif