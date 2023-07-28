/*
 * vKernel_Cfg.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */

#ifndef VOS_CFG_H_
#define VOS_CFG_H_

#include "vOs.h"
#include "vOsHook.h"

#define VOS_CFG_SYSTICK_MS 1 //lower is better for window CPU loading

#define VOS_CFG_IDLEHOOK OsIdleHook

#define VOS_CFG_DEBUG_ENABLED

/* Define Event */
#define vOsEvent0      1
#define vOsEvent1      2

#endif /* VOS_CFG_H_ */
