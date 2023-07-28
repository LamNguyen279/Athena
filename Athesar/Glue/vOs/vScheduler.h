/*
 * vScheduler.h
 *
 *  Created on: Jul 22, 2023
 *      Author: Lam Nguyen
 */

#ifndef VSCHEDULER_H_
#define VSCHEDULER_H_

#include "vTask.h"

extern void vSchedulerInit(void);

extern void vSchedulerStart(void);

extern void vSchedulerStop(void);

extern void vSchedulerTerminateTask(void);

extern void vSchedulerAddTask(vTaskHandler_t *task);

extern void vSchedulerWaitForSchedulePoint(void);

extern vTaskHandler_t *vSchedulerCurTask;
extern vTaskHandler_t *vSchedulerGetCurRunTask(void);

#endif /* VSCHEDULER_H_ */
