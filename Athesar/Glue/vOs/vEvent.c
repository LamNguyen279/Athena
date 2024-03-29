/*
 * vEvent.c
 *
 *  Created on: Jul 23, 2023
 *      Author: ADMIN
 */
#include "vTask.h"
#include "vEvent.h"
#include "vScheduler.h"

static void *_vEventMutex;

void vEventInit()
{
  _vEventMutex = CreateMutex(VOS_NULL, VOS_FALSE, VOS_NULL);
}

void vEventSet(vTaskHandler_t *task, vEventMask_t eventMask)
{
  WaitForSingleObject( _vEventMutex, INFINITE );
  //check task are in waiting event state
  if((task->State == VTASK_STATE_WAIT) && (task->SubState == VTASK_SUBSTATE_WAIT_EVENT))
  {
    //check task are waiting for this event
    if(((task->WaitMask) & eventMask) == eventMask)
    {
      //one or more events will be set according to mask
      task->WaitValue |= eventMask;
      //push the task to ready list
      vSchedulerAddTask(task);

      vSchedulerWaitForSchedulePoint();
    }
  }

  ReleaseMutex( _vEventMutex );
}

void vEventGet(vTaskHandler_t *task, vEventMask_t *eventValue)
{
  *eventValue = task->WaitValue;
}

void vEventWait(vEventMask_t eventMask)
{
  vTaskHandler_t *curTask = vSchedulerGetCurRunTask();

  //one or more event not set
  if(((curTask->WaitValue) & (eventMask)) != eventMask)
  {
    //set the mask for waiting event
    curTask->WaitMask |= eventMask;
    //wait schedule point
    vSchedulerWaitForSchedulePoint();
  }
}

void vEventClear(vEventMask_t eventMask)
{
  vTaskHandler_t *curTask = vSchedulerGetCurRunTask();

  if(((curTask->WaitValue) & (eventMask)) != 0)
  {
    //clear value at the position of the mask
    curTask->WaitValue &= ~eventMask;
    curTask->WaitMask &= ~eventMask;
  }

}
