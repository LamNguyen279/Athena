/*
 * vAlarm.c
 *
 *  Created on: Aug 15, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "vAlarm.h"
#include "stdlib.h"
#include "vOs.h"
#include "vTask.h"
/* ***************************** [ MACROS    ] ****************************** */
#define IS_INFINITY(alarm)        ( (alarm)->SetTick == VLARM_INFINITY  )
#define IS_EXPIRED(alarm)         ( (alarm)->CurTick == (alarm)->SetTick )
#define IS_RUNNING(alarm)         ( ((alarm)->CurTick < ((alarm)->SetTick)) && (0 <= (alarm)->CurTick))
#define INCREASE(alarm)           ( (alarm)->CurTick++ )
/* ***************************** [ TYPES     ] ****************************** */
typedef void (*vAlarmCbkFnc)(void);
/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */
vAlarmHandler_t *vAlarm;
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
vAlarmHandler_t *vAlarmCreate(vAlarmPar_t *AlarmPar)
{
  vAlarmHandler_t *newAlarm;

  if(AlarmPar != VOS_NULL)
  {
    newAlarm = (vAlarmHandler_t *)malloc(sizeof(vAlarmHandler_t));

    newAlarm->ReloadType = AlarmPar->ReloadType;

    newAlarm->Type = AlarmPar->AlarmType;

    newAlarm->SetTick = AlarmPar->SetTick;

    newAlarm->Action = AlarmPar->Action;

    if(newAlarm->ReloadType == VALARM_AUTO)
    {
      newAlarm->CurTick = 0;
    }else
    {
      newAlarm->CurTick = newAlarm->SetTick + 1;
    }

    newAlarm->NextAlarm = vAlarm;

    vAlarm = newAlarm;
  }

  return newAlarm;
}

vOsStatusType vAlarmStart(vAlarmHandler_t *this)
{
  vOsStatusType ret = VOS_OK;

  if((this == VOS_NULL) && (this->ReloadType != VALARM_AUTO))
  {
    ret = VOS_NOT_OK;
  }else
  {
    this->CurTick = 0;
  }

  return ret;
}

vOsStatusType vAlarmStop(vAlarmHandler_t *this)
{
  vOsStatusType ret = VOS_OK;

  if((this == VOS_NULL) && (this->ReloadType != VALARM_AUTO))
  {
    ret = VOS_NOT_OK;
  }else
  {
    this->CurTick = this->SetTick + 1;
  }

  return ret;
}

void vAlarmScheduler(void)
{
  vAlarmHandler_t *alarm = vAlarm;
  vTaskHandler_t *task;
  vAlarmCbkFnc alarmCbk;

  while(alarm != VOS_NULL)
  {
    if(IS_EXPIRED(alarm))
    {
      if(alarm->ReloadType == VALARM_AUTO)
      {
        alarm->CurTick = 0;
      }else
      {
        alarm->CurTick += 1;
      }

      if(alarm->Action != VOS_NULL) //check and execute alarm action
      {
        switch(alarm->Type)
        {
          case VALARM_ACTION_ACTIVATE_TASK:
            task = (vTaskHandler_t *)(alarm->Action);
            vTaskActivate(task);
            break;
          case VALARM_ACTION_CALLBACK:
            alarmCbk = (vAlarmCbkFnc)(alarm->Action);
            alarmCbk();
            break;
          case VALARM_ACTION_INCREASE_COUNTER:
            break;
          case VALARM_ACTION_NON:
            break;
          default:
            break;
        }
      }
    }

    if(IS_RUNNING(alarm))
    {
      INCREASE(alarm);
    }

    alarm = alarm->NextAlarm;
  }
}
