/*
 * SoAd_Cfg.h
 *
 *  Created on: Jul 31, 2023
 *      Author: ADMIN
 */

#ifndef SOAD_SOAD_CFG_H_
#define SOAD_SOAD_CFG_H_

/* ***************************** [ INCLUDES  ] ****************************** */
#include "Std_Types.h"
#include "SoAd_Priv.h"
/* ***************************** [ MACROS    ] ****************************** */
#define SOAD_CFG_NUM_SOCON_GROUP                      20

#define SOAD_CFG_NUM_ROUTING_GROUP                    10

#define SOAD_CFG_NUM_SOCON                            20

#define SOAD_CFG_SOCON_RX_BUFF_SIZE                   256

#define SOAD_CFG_SOCON_TX_BUFF_SIZE                   256

#define SOAD_CFGDET_USED                              STD_OFF

#define SOAD_CFG_GETANDRESET_MEASUREMENTDATA_API      STD_OFF

#define SOAD_CFG_IPV6_ENABLED                         STD_OFF

#define SOAD_CFG_CERT_MSG_ENABLED                     STD_OFF

#define SOAD_CFG_MAINFUNCTION_PERIOD                  10

#define SOAD_CFG_VERSIONINFO_API                      STD_OFF

#define SOAD_CFG_PDUHEADER_BYTE_ORDER                 SOAD_BIG_ENDIAN
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
extern const SoAd_CfgPduRoute_t SoAd_PduRouteArr[];
extern const uint32 SoAd_PduRouteArrSize;

extern const SoAd_CfgPduRouteDest_t SoAd_PduRouteDestArr[];
extern const uint32 SoAd_PduRouteDestArrSize;

extern const SoAd_CfgRoutingGroup_t SoAd_RoutingGroupArr[];
extern const uint32 SoAd_RoutingGroupArrSize;

extern const SoAd_CfgSoConGrp_t SoAd_SoConGrpArr[];
extern const uint32 SoAd_SoConGrpArrSize;

extern const SoAd_CfgSoCon_t SoAd_SoConArr[];
extern const uint32 SoAd_SoConArrSize;

extern const SoAd_CfgSocketRoute_t SoAd_SocketRouteArr[];
extern const uint32 SoAd_CfgSocketRouteArrSize;
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_CFG_H_ */