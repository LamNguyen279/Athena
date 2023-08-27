#include "DoIP_IntTypes.h"

DoIP_EntityCfgType DoIP_EntityPreCfg;
DoIP_InterfaceCfgType* DoIP_InterfacePreCfg;
DoIP_ChannelCfgType* DoIP_ChannelPreCfg[DOIP_NUM_CHAN];
DoIP_ConCfgType* DoIP_TcpConPreCfg[DOIP_NUM_TCP_CON];
DoIP_ConCfgType* DoIP_UdpConPreCfg[DOIP_NUM_UDP_CON];
DoIP_ConCfgType* DoIP_AnnConPreCfg[DOIP_NUM_ANN_CON];
DoIP_RoutActCfgType* DoIP_RoutActPreCfg[DOIP_NUM_ROUT_ACT];
DoIP_TesterCfgType* DoIP_TesterPreCfg[DOIP_NUM_TESTER];