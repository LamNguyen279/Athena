/*
 * vTask.c
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */


#include "vTask.h"
#include "vScheduler.h"
#include "stdlib.h"

static void *_vTaskMutex;

static vTaskHandler_t *_vTaskAllocate(void);
static void _vTaskFree(vTaskHandler_t *task);

static uint8_t vTaskInitFlag = VOS_FALSE;

void vTaskInit(void)
{
  vTaskInitFlag = VOS_TRUE;
  _vTaskMutex = CreateMutex(VOS_NULL, VOS_FALSE, VOS_NULL);
}

vTaskHandler_t *vTaskCreate(
vTaskFuncEntry_t Entry,
vTaskPriority_t Priority,
vTaskPreemptibility_t PreempMode)
{
  vTaskHandler_t *vNewTask = VOS_NULL;

  if(vTaskInitFlag == VOS_TRUE)
  {
    vNewTask = _vTaskAllocate();

    if(vNewTask != VOS_NULL)
    {
      //new thread with suspended state
      vNewTask->W32Thread.Hdl = CreateThread(
          NULL, 1024,
          ( LPTHREAD_START_ROUTINE ) Entry,
          VOS_NULL, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
          &(vNewTask->W32Thread.Id) );

      if(vNewTask->W32Thread.Hdl)
      {
  //      SetThreadAffinityMask( vNewTask->W32Thread.Hdl, 0x01 );
//        SetThreadPriorityBoost( vNewTask->W32Thread.Hdl, TRUE );
//        SetThreadPriority( vNewTask->W32Thread.Hdl, THREAD_PRIORITY_TIME_CRITICAL );
        vNewTask->State = VTASK_STATE_SUSPENDED;
        vNewTask->SubState = VTASK_SUBSTATE_SUSPENDED_NEW;

        //static
        vNewTask->Priority = Priority;
        vNewTask->FuncEntry = Entry;
        vNewTask->PreempMode = PreempMode;
      }else
      {
        _vTaskFree(vNewTask);
      }

    }
  }

  return vNewTask;
}

void vTaskActivate(vTaskHandler_t *task)
{
  WaitForSingleObject( _vTaskMutex, INFINITE );

  vTaskHandler_t *thisTask = task;

  if(thisTask->State == VTASK_STATE_READY)
  {
    //reactivate, TODO: raise error hook if needed
  }else if(thisTask->State == VTASK_STATE_RUNNING)
  {
    //reactivate, TODO: raise error hook if needed
  }else if(thisTask->State == VTASK_STATE_SUSPENDED)
  {
    if(thisTask->SubState == VTASK_SUBSTATE_SUSPENDED_NEW)
    {
      thisTask->State = VTASK_STATE_READY;
      thisTask->SubState = VTASK_SUBSTATE_READY_NEW;

    }else if(thisTask->SubState == VTASK_SUBSTATE_SUSPENDED_AGAIN)
    {
      thisTask->State = VTASK_STATE_READY;
      thisTask->SubState = VTASK_SUBSTATE_READY_AGAIN;
    }

    vSchedulerAddTask(thisTask);
  }else //-> VTASK_STATE_WAIT for event
  {
    //reject
  }

  ReleaseMutex( _vTaskMutex );
}

void vTaskTicksDelay(uint32_t ticks)
{
  Sleep(VOS_CFG_SYSTICK_MS * ticks);
}

void vTaskTerminate()
{
  WaitForSingleObject( _vTaskMutex, INFINITE );
  vSchedulerTerminateTask();
  ReleaseMutex( _vTaskMutex );
}

static vTaskHandler_t *_vTaskAllocate(void)
{
  return (vTaskHandler_t *)malloc(sizeof(vTaskHandler_t ));
}

static void _vTaskFree(vTaskHandler_t *task)
{
  free(task);
}
