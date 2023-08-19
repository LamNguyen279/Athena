/*
 * vOs_Global.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */

#ifndef VOS_H_
#define VOS_H_

#include "vOs_Cfg.h"
#include "stdint.h"
#include "stdbool.h"

#ifndef VOS_NULL
#define VOS_NULL  NULL
#endif

#ifndef VOS_FALSE
#define VOS_FALSE  false
#endif

#ifndef VOS_TRUE
#define VOS_TRUE  true
#endif

typedef uint8_t vOsStatusType;
#define VOS_OK            ((vOsStatusType)0)
#define VOS_NOT_OK        ((vOsStatusType)1)

typedef void (*vOsHook_t)(void);

#ifdef VOS_CFG_DEBUG_ENABLED
#include "stdio.h"
#define VOS_CONSOLELOG(SomeString, SomeThing) \
  printf("LINE: %d, FUNC: %s, LOG: ", __LINE__, __func__); \
  printf((SomeString), (SomeThing)); \
  printf("\n"); \
  fflush(stdout)
#else
#define VOS_CONSOLELOG(SomeString, SomeThing)
#endif

#endif /* VOS_H_ */
