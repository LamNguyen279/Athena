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
#define VISR_QUEUE_DEPTH  256
/* ***************************** [ TYPES     ] ****************************** */
typedef struct _vIsrHandler_t
{
  //static
  void (*Cbk) (void);
  uint32_t Mask;
  struct _vIsrHandler_t *NextIsr;
} vIsrHandler_t;

typedef struct _vIsrMask_t
{
  uint32_t Mask;
} vIsrQueueData_t;

typedef struct _vIsrQueueData_t
{
  vIsrQueueData_t IsrData[VISR_QUEUE_DEPTH];
  uint32_t front;
  uint32_t rear;
  uint32_t size;
} vIsrQueue_t;
/* ***************************** [ DECLARES  ] ****************************** */
static void vIsrNonBlkThreadGetChar(void);
static void vIsrKeyboardRoutine(void);

static void vIsrInitQueue(vIsrQueue_t *queue);
static boolean vIsrEnqueueElement(vIsrQueue_t *queue, vIsrQueueData_t *buffer);
static boolean vIsrGetQueue1stElement(vIsrQueue_t *queue, vIsrQueueData_t *dest);
static boolean vIsrRemoveQueue1stElement(vIsrQueue_t *queue);
/* ***************************** [ DATAS     ] ****************************** */
static vIsrHandler_t *vIsr = VOS_NULL;

static vAlarmHandler_t *vIsrAlarm;

static vIsrQueue_t vIsrQueue;
/* ***************************** [ LOCALS    ] ****************************** */
static void vIsrNonBlkThreadGetChar(void)
{
  int chPressed = -1;
  vIsrQueueData_t isrElement;

  if (_kbhit())
  {
      chPressed = _getch();
      isrElement.Mask = chPressed;
      vIsrEnqueueElement(&vIsrQueue, &isrElement);

//      Self call for store all data to queue
      vIsrNonBlkThreadGetChar();
  }
}

static void vIsrKeyboardRoutine(void)
{
   vIsrNonBlkThreadGetChar();
}

static void vIsrInitQueue(vIsrQueue_t *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
}

static boolean vIsrEnqueueElement(vIsrQueue_t *queue, vIsrQueueData_t *buffer)
{
    if((queue->size) == VISR_QUEUE_DEPTH)
    {
        return FALSE; // Queue is full, unable to enqueue
    }

    queue->rear = (queue->rear + 1) % VISR_QUEUE_DEPTH;
    queue->IsrData[queue->rear] = *buffer;
    queue->size++;
    return TRUE;
}

static boolean vIsrGetQueue1stElement(vIsrQueue_t *queue, vIsrQueueData_t *dest)
{
    if((queue->size) == 0)
    {
        return FALSE; // Queue is empty, unable to get first element
    }

    *dest = queue->IsrData[queue->front];
    return TRUE;
}

static boolean vIsrRemoveQueue1stElement(vIsrQueue_t *queue)
{
    if ((queue->size) == 0) {
        return FALSE; // Queue is empty, unable to remove first element
    }

    queue->front = (queue->front + 1) % VISR_QUEUE_DEPTH;
    queue->size--;
    return TRUE;
}
/* ***************************** [ FUNCTIONS ] ****************************** */
void vIsrInit(void)
{
  vIsrInitQueue(&vIsrQueue);

  vAlarmPar_t isrAlarmPar = {
      VALARM_AUTO,
      1,
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
  vIsrQueueData_t vIsrData;

  //check that some ISR occured And Handle them
  if(vIsrGetQueue1stElement(&vIsrQueue, &vIsrData) == TRUE)
  {
    vIsrHandler_t *this = vIsr;
    while(this != VOS_NULL)
    {
      if((this->Mask) == (vIsrData.Mask))
      {
        if(this->Cbk != VOS_NULL)
        {
          vTaskHandler_t *curTask = vSchedulerGetCurRunTask();

          if(curTask != VOS_NULL)
          {
            SuspendThread(curTask->W32Thread.Hdl);
            this->Cbk();
            ResumeThread(curTask->W32Thread.Hdl);
          }else
          {
            this->Cbk();
          }
        }
      }

      this = this->NextIsr;
    }

    vIsrRemoveQueue1stElement(&vIsrQueue);
  }
}

