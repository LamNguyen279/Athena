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
#define VOS_CONSOLELOG(someString, someThing) \
  printf((someString),(someThing)); \
  fflush(stdout)
#else
#define VOS_CONSOLELOG(someString, someThing)
#endif

#endif /* VOS_H_ */
