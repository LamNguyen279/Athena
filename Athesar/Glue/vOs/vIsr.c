/*
 * vIsr.c
 *
 *  Created on: Aug 19, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include <stdio.h>
#include "vOs.h"
#include "stdlib.h"
#include "vIsr.h"
#include <windows.h>
#include "vScheduler.h"
#include <conio.h>

#include "vAlarm.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
typedef struct _vIsrHandler_t
{
  //static
  void (*Cbk) (void);
  uint32_t Mask;
  struct _vIsrHandler_t *NextIsr;
} vIsrHandler_t;
/* ***************************** [ DECLARES  ] ****************************** */
static int vIsrGetch(void);
static void vIsrKeyboardRoutine(void);
/* ***************************** [ DATAS     ] ****************************** */
static vIsrHandler_t *vIsr = VOS_NULL;
static uint32_t vIsrMask = -1;
static DWORD vIsrThreadId;
static HANDLE vIsrThreadHdl;

vTaskHandler_t *vIsrTask;
vAlarmHandler_t *vIsrAlarm;
/* ***************************** [ LOCALS    ] ****************************** */
int NonBlockingGetChar() {
  int ret = -1;

  if (_kbhit())
  {
      ret = _getch();
      NonBlockingGetChar();
  }

  return ret;
}

static void vIsrKeyboardRoutine(void)
{
   vIsrMask = NonBlockingGetChar();
}
/* ***************************** [ FUNCTIONS ] ****************************** */
void vIsrInit(void)
{
  vAlarmPar_t isrAlarmPar = {
      VALARM_AUTO,
      10,
      VALARM_ACTION_CALLBACK,
      (void *)vIsrKeyboardRoutine,
  };

  vIsrAlarm = vAlarmCreate(&isrAlarmPar);
}

void vIsrCreate(int vIsrMask, void *IsrEntry)
{
  vIsrHandler_t *newIsr = (vIsrHandler_t *)malloc(sizeof(vIsrHandler_t));

  newIsr->Cbk = IsrEntry;
  newIsr->Mask = vIsrMask;

  newIsr->NextIsr = vIsr;
  vIsr = newIsr;
}

void vIsrScheduler(void)
{
  vIsrHandler_t *thisIsr = vIsr;

  while(thisIsr != VOS_NULL)
  {
    if(thisIsr->Mask == vIsrMask)
    {
      if(thisIsr->Cbk != VOS_NULL)
      {
        vTaskHandler_t *curTask = vSchedulerGetCurRunTask();

        if(curTask != VOS_NULL)
        {
          SuspendThread(curTask->W32Thread.Hdl);
          thisIsr->Cbk();
          ResumeThread(curTask->W32Thread.Hdl);
        }else
        {
          thisIsr->Cbk();
        }

        vIsrMask = -1;
      }
    }

    thisIsr = thisIsr->NextIsr;
  }
}

