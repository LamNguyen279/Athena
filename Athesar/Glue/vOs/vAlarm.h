/*
 * vAlarm.h
 *
 *  Created on: Aug 15, 2023
 *      Author: ADMIN
 */
#ifndef VALARM_H_
#define VALARM_H_
/* ***************************** [ INCLUDES  ] ****************************** */
#include "stdint.h"
#include "stdbool.h"
/* ***************************** [ MACROS    ] ****************************** */

#define VLARM_INFINITY  ((vAlarmTick_t)-1)

/* ***************************** [ TYPES     ] ****************************** */
typedef uint32_t vAlarmTick_t;

typedef enum _vAlarmAction_t
{
  VALARM_ACTION_NON = 0,
  VALARM_ACTION_ACTIVATE_TASK = 1,
  VALARM_ACTION_CALLBACK,
  VALARM_ACTION_INCREASE_COUNTER
} vAlarmAction_t;

typedef enum _vAlarmReload_t
{
  VALARM_AUTO = 1,
  VALARM_MANUAL
} vAlarmReload_t;

typedef struct _vAlarmHandler_t
{
  vAlarmTick_t CurTick;
  struct _vAlarmHandler_t *NextAlarm;
  //static
  vAlarmReload_t ReloadType;
  vAlarmTick_t SetTick;
  vAlarmAction_t Type;
  void *Action;
} vAlarmHandler_t;

typedef struct _vAlarmPar_t
{
  vAlarmReload_t ReloadType;
  vAlarmTick_t SetTick;
  vAlarmAction_t AlarmType;
  void *Action;
} vAlarmPar_t;

/* ***************************** [ DECLARES  ] ****************************** */
vAlarmHandler_t *vAlarmCreate(vAlarmPar_t *AlarmPar);

void vAlarmScheduler(void);
/* ***************************** [ DATAS     ] ****************************** */
extern vAlarmHandler_t *vAlarm;
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
#endif
