/*
 * vTask.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */

#ifndef VTASK_H_
#define VTASK_H_

#include <windows.h>
#include "stdint.h"
#include "vOs_Cfg.h"

/* macro */

#ifndef VOS_NULL
#define VOS_NULL  NULL
#endif

#ifndef VOS_FALSE
#define VOS_FALSE  FALSE
#endif


/* types */
typedef enum vTaskState_t
{
  VTASK_STATE_READY = 1,
  VTASK_STATE_RUNNING,
  VTASK_STATE_SUSPENDED,
  VTASK_STATE_WAIT
} vTaskState_t;

typedef enum vTaskSubState_t
{
  VTASK_SUBSTATE_SUSPENDED_NEW = 1,
  VTASK_SUBSTATE_READY_NEW,
  VTASK_SUBSTATE_RUNING_NEW,
  VTASK_SUBSTATE_READY_PAUSE,
  VTASK_SUBSTATE_RUNING_AGAIN,
  VTASK_SUBSTATE_SUSPENDED_AGAIN,
  VTASK_SUBSTATE_READY_AGAIN,
  VTASK_SUBSTATE_WAIT_EVENT
} vTaskSubState_t;

typedef enum _vTaskPreemptibility_t
{
  VTASK_PREEMP_FULL = 1,
  VTASK_PREEMP_NON
} vTaskPreemptibility_t;

typedef uint8_t vTaskPriority_t;
typedef void* vTaskFuncEntry_t;
typedef uint32_t vTaskEventMask_t;

typedef struct _vTaskHandler_t
{
  struct W32Thread_t
  {
    HANDLE Hdl;
    DWORD Id;
  } W32Thread;
  vTaskState_t State;
  vTaskSubState_t SubState;
  vTaskEventMask_t WaitValue;
  //static
  vTaskEventMask_t WaitMask;
  vTaskPriority_t Priority;
  vTaskFuncEntry_t FuncEntry;
  vTaskPreemptibility_t PreempMode;
} vTaskHandler_t;

typedef struct _vTaskCreatePar_t
{
  vTaskFuncEntry_t Entry;
  vTaskPriority_t Priority;
} vTaskCreatePar_t;

extern void vTaskInit(void);

extern vTaskHandler_t *vTaskCreate(
vTaskFuncEntry_t Entry,
vTaskPriority_t Priority,
vTaskPreemptibility_t PreempMode);

extern void vTaskActivate(vTaskHandler_t *task);

extern void vTaskTerminate();

extern void vTaskTickDelay(uint32_t ms);

#endif /* VTASK_H_ */
