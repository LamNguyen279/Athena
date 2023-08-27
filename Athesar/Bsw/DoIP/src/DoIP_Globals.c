#include "DoIP_Globals.h"
//#include "DoIP_Cfg.h"
//#include "DoIP_IntTypes.h"

uint8 DoIP_ModuleStatus = DOIP_MODULE_STATUS_DEFAULT;

DoIP_EntityDataType DoIP_EntityData;
DoIP_InterfaceDataType DoIP_InterfaceData[DOIP_NUM_INTERFACE];
DoIP_ChannelDataType DoIP_ChannelData[DOIP_NUM_CHAN];
DoIP_TcpConDataType DoIP_TcpConData[DOIP_NUM_TCP_CON];
DoIP_UdpConDataType DoIP_UdpConData[DOIP_NUM_UDP_CON];
DoIP_AnnConDataType DoIP_AnnConData[DOIP_NUM_ANN_CON];
DoIP_RoutActDataType DoIP_RoutActData[DOIP_NUM_ROUT_ACT];
DoIP_TesterDataType DoIP_TesterData[DOIP_NUM_TESTER];