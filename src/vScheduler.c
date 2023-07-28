/*
 * vScheduler.c
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */

#include "vOs.h"
#include "vScheduler.h"
#include "vTask.h"
#include "vOs_Cfg.h"

/*****************************************************************************
 * Private types
 ****************************************************************************/

typedef enum _vSchedulerState_t
{
  VSCHEDULER_STATE_STOP = 0,
  VSCHEDULER_STATE_RUNNING,
  VSCHEDULER_STATE_WAIT
} vSchedulerState_t;

typedef struct _ReadyTaskNode_t
{
  //struct _ReadyTaskNode_t *PreTaskNode;
  struct _ReadyTaskNode_t *NextTaskNode;
  vTaskHandler_t *Task;
} vSchedulerTasksNode_t;

typedef struct _vScheduler_t
{
  vSchedulerState_t State;
  vTaskHandler_t *CurTask;
  vSchedulerTasksNode_t *ReadyTasks;
} vScheduler_t;

/*****************************************************************************
 * Private variables
 ****************************************************************************/
static void *_vSchedulerMutex;
static vOsHook_t _vSchedulerIdeHookFncPtr;
static vScheduler_t _vScheduler;
vTaskHandler_t *vSchedulerCurTask;

/*****************************************************************************
 * Private function definition
 ****************************************************************************/
static void _vSchedulerEnqueueTask(vTaskHandler_t *task);
static vTaskHandler_t *_vSchedulerDequeueTask(void);
static void _vShedulerHandleTask(void);
static void _vSchedulerHandleIsr(void);
static void _vSchedulerMainFunction(void);

/*****************************************************************************
 * Public function declaration
 ****************************************************************************/
void vSchedulerInit(void)
{
  _vScheduler.ReadyTasks = VOS_NULL;
  _vSchedulerIdeHookFncPtr = &VOS_CFG_IDLEHOOK;

  _vSchedulerMutex = CreateMutex(VOS_NULL, VOS_FALSE, VOS_NULL);
}

void vSchedulerStart(void)
{
  _vScheduler.State = VSCHEDULER_STATE_RUNNING;

  SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  SetProcessAffinityMask( GetCurrentProcess(), 1 );

  while(1)
  {
    WaitForSingleObject( _vSchedulerMutex, INFINITE );

    Sleep(VOS_CFG_SYSTICK_MS);
    _vSchedulerMainFunction();

    ReleaseMutex( _vSchedulerMutex );
  }
}

void vSchedulerStop(void)
{
  _vScheduler.State = VSCHEDULER_STATE_STOP;
}

void vSchedulerAddTask(vTaskHandler_t *task)
{
  _vSchedulerEnqueueTask(task);
}

vTaskHandler_t *vSchedulerGetCurRunTask(void)
{
  vSchedulerCurTask = _vScheduler.CurTask;

  return vSchedulerCurTask;
}

void vSchedulerWaitForSchedulePoint(void)
{
  _vScheduler.State = VSCHEDULER_STATE_WAIT;
  vTaskTickDelay(1);
  while(_vScheduler.State == VSCHEDULER_STATE_WAIT)
  {
  }
}
/*****************************************************************************
 * Private function declaration
 ****************************************************************************/
static void _vSchedulerEnqueueTask(vTaskHandler_t *task)
{
  vSchedulerTasksNode_t *newTaskNode = (vSchedulerTasksNode_t *)malloc(sizeof(vSchedulerTasksNode_t));
  newTaskNode->Task = task;
//  newTaskNode->PreTaskNode = VTASK_NULL;
  newTaskNode->NextTaskNode = VOS_NULL;

  if(_vScheduler.ReadyTasks == VOS_NULL)
  {
    //empty list
    _vScheduler.ReadyTasks = newTaskNode;
  }else
  {
    vSchedulerTasksNode_t *tmpTaskNode = VOS_NULL;

    //new task is highest priority
    if(newTaskNode->Task->Priority >= _vScheduler.ReadyTasks->Task->Priority)
    {
      tmpTaskNode = _vScheduler.ReadyTasks;
      newTaskNode->NextTaskNode = tmpTaskNode;

      _vScheduler.ReadyTasks = newTaskNode;
    }else
    {
      vSchedulerTasksNode_t *browseTaskNode = _vScheduler.ReadyTasks;

      if(browseTaskNode->NextTaskNode == VOS_NULL)
      {
        //second task
        browseTaskNode->NextTaskNode = newTaskNode;
      }else
      {
        //>= 3rd Task
        while(browseTaskNode != VOS_NULL)
        {
          //insert at the last of list
          if(browseTaskNode->NextTaskNode == VOS_NULL)
          {
            browseTaskNode->NextTaskNode = newTaskNode;
            break;

          }else //insert at the middle of list
          {
            if(newTaskNode->Task->Priority >= browseTaskNode->NextTaskNode->Task->Priority)
            {
              tmpTaskNode = browseTaskNode->NextTaskNode;
              newTaskNode->NextTaskNode = tmpTaskNode;

              browseTaskNode->NextTaskNode = newTaskNode;
              break;
            }
          }

          //next browsing
          browseTaskNode = browseTaskNode->NextTaskNode;
        }
      }
    }
  }
}

static vTaskHandler_t *_vSchedulerDequeueTask(void)
{
  vTaskHandler_t *retTask = VOS_NULL;

  /* Check the highest priority task at this time */

  //no any task is running
  if(_vScheduler.CurTask == VOS_NULL)
  {
    if(_vScheduler.ReadyTasks != VOS_NULL)
    {
      //get first task in the ready list
      _vScheduler.CurTask = _vScheduler.ReadyTasks->Task;

      vSchedulerTasksNode_t *freeTaskNode = _vScheduler.ReadyTasks;
      _vScheduler.ReadyTasks = _vScheduler.ReadyTasks->NextTaskNode;

      //free memory linked list
      free(freeTaskNode);

      retTask = _vScheduler.CurTask;
    }else //no any task to be switched
    {
      //IDLE, call IDLE hook function
      if(_vSchedulerIdeHookFncPtr != VOS_NULL)
      {
        _vSchedulerIdeHookFncPtr();
      }
    }
  }else
  {
    //this task is waiting for events, change it to WAIT state, set event will activate task again
    if( (_vScheduler.CurTask->WaitMask != 0) &&
       ( ((_vScheduler.CurTask->WaitMask) ^ (_vScheduler.CurTask->WaitValue)) != 0 ))
    {
      vTaskHandler_t *suspendTask = _vScheduler.CurTask;
      SuspendThread(suspendTask->W32Thread.Hdl);

      _vScheduler.CurTask->State = VTASK_STATE_WAIT;
      _vScheduler.CurTask->SubState = VTASK_SUBSTATE_WAIT_EVENT;

      _vScheduler.CurTask = VOS_NULL;

      if(_vScheduler.ReadyTasks != VOS_NULL)
      {
        _vScheduler.CurTask = _vScheduler.ReadyTasks->Task;

        vSchedulerTasksNode_t *freeTaskNode = _vScheduler.ReadyTasks;
        _vScheduler.ReadyTasks = _vScheduler.ReadyTasks->NextTaskNode;
        free(freeTaskNode);
      }

      retTask = _vScheduler.CurTask;
    }

    //Task is running with Preemptive mode FULL => put it to READY state
    if((_vScheduler.CurTask != VOS_NULL) && (_vScheduler.CurTask->PreempMode == VTASK_PREEMP_FULL))
    {
      //check if any task in ready list
      if((_vScheduler.ReadyTasks != VOS_NULL) &&
          (_vScheduler.CurTask->Priority < _vScheduler.ReadyTasks->Task->Priority))
      {
        vTaskHandler_t *suspendTask = _vScheduler.CurTask;
        SuspendThread(suspendTask->W32Thread.Hdl);

        _vScheduler.CurTask->State = VTASK_STATE_READY;
        _vScheduler.CurTask->SubState = VTASK_SUBSTATE_READY_PAUSE;

        _vScheduler.CurTask = _vScheduler.ReadyTasks->Task;

        vSchedulerTasksNode_t *freeTaskNode = _vScheduler.ReadyTasks;
        _vScheduler.ReadyTasks = _vScheduler.ReadyTasks->NextTaskNode;

        free(freeTaskNode);

        retTask = _vScheduler.CurTask;

        _vSchedulerEnqueueTask(suspendTask);
      }else
      {
        //cur task is highest priority in the list, let it run
      }
    }
  }

  return retTask;
}

void vSchedulerTerminateTask(void)
{
  vTaskHandler_t *task = _vScheduler.CurTask;

  if(task->W32Thread.Id == GetCurrentThreadId())
  {
    task->State = VTASK_STATE_SUSPENDED;
    task->SubState = VTASK_SUBSTATE_SUSPENDED_AGAIN;
    _vScheduler.CurTask = VOS_NULL;

    VOS_CONSOLELOG("Terminate Task: %d\n", task->W32Thread.Id);

    TerminateThread(task->W32Thread.Hdl, 0);
  }else
  {
    //TODO, error terminate from another thread
  }
}

static void _vShedulerHandleTask(void)
{
  vTaskHandler_t *task = _vSchedulerDequeueTask();
  if(task != VOS_NULL)
  {
    switch(task->State)
    {
      case VTASK_STATE_READY: //READY -> RUN

        task->State = VTASK_STATE_RUNNING;
        //check this task is new or paused in previous arbitration
        if(task->SubState == VTASK_SUBSTATE_READY_NEW)
        {
          task->SubState = VTASK_SUBSTATE_RUNING_NEW;

        }else if(task->SubState == VTASK_SUBSTATE_READY_PAUSE)
        {
          task->SubState = VTASK_SUBSTATE_RUNING_AGAIN;
        }else //VTASK_SUBSTATE_READY_AGAIN
        {
          //create thread again
          task->W32Thread.Hdl = CreateThread(
           NULL, 1024,
           ( LPTHREAD_START_ROUTINE ) task->FuncEntry,
           VOS_NULL, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION,
           &(task->W32Thread.Id) );

          if(task->W32Thread.Hdl)
          {
            SetThreadAffinityMask( task->W32Thread.Hdl, 0x01 );
            SetThreadPriorityBoost( task->W32Thread.Hdl, TRUE );
            SetThreadPriority( task->W32Thread.Hdl, THREAD_PRIORITY_TIME_CRITICAL );
            task->SubState = VTASK_SUBSTATE_RUNING_NEW;
          }
        }
        ResumeThread(task->W32Thread.Hdl);

        break;
      case VTASK_STATE_WAIT: //WAIT -> RUN

        task->State = VTASK_STATE_RUNNING;
        task->SubState = VTASK_SUBSTATE_RUNING_AGAIN;
        ResumeThread(task->W32Thread.Hdl);

        break;
      default:
        break;
    }

  }
}

static void _vSchedulerHandleIsr(void)
{

}

static void _vSchedulerMainFunction(void)
{
  _vSchedulerHandleIsr();
  _vShedulerHandleTask();

  _vScheduler.State = VSCHEDULER_STATE_RUNNING;
}

/*****************************************************************************
 * End of file
 ****************************************************************************/
