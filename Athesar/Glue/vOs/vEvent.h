/*
 * vEvent.h
 *
 *  Created on: Jul 23, 2023
 *      Author: ADMIN
 */

#ifndef VEVENT_H_
#define VEVENT_H_

typedef uint32_t vEventMask_t;

extern void vEventSet(vTaskHandler_t *task, vEventMask_t eventMask);
extern void vEventGet(vTaskHandler_t *task, vEventMask_t *eventValue);
extern void vEventWait(vEventMask_t eventMask);
extern void vEventClear(vEventMask_t eventMask);

#endif /* VEVENT_H_ */
