/*
 * SoAd_Glob.h
 *
 *  Created on: Jul 31, 2023
 *      Author: ADMIN
 */

#ifndef SOAD_SOAD_RAM_H_
#define SOAD_SOAD_RAM_H_

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd_Priv.h"
#include "SoAd_Cfg.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
uint32_t SoAd_DynSoConArrCtn;

SoAd_SoCon_t SoAd_DynSoConArr[SOAD_CFG_NUM_SOCON];
SoAdSoConGr_t SoAd_DynSoConGrArr[SOAD_CFG_NUM_SOCON_GROUP];

SoAd_CfgUpperFncTable_t SoAd_UpperFunctionTable[];
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* SOAD_SOAD_RAM_H_ */
