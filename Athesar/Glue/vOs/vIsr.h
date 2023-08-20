/*
 * vIsr.h
 *
 *  Created on: Aug 19, 2023
 *      Author: ADMIN
 */

#ifndef VOS_VISR_H_
#define VOS_VISR_H_

/* ***************************** [ INCLUDES  ] ****************************** */
/* ***************************** [ MACROS    ] ****************************** */
#define VISR_MASK_KEY_A     'A'
#define VISR_MASK_KEY_1     '1'
#define VISR_MASK_KEY_2     '2'
#define VISR_MASK_KEY_3     '3'

#define VISR_CAT2_STACK_SIZE  15000
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
extern void vIsrCreate(int vIsrMask, void *IsrEntry);
extern void vIsrScheduler(void);
extern void vIsrInit(void);
/* ***************************** [ DATAS     ] ****************************** */
/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */

#endif /* VOS_VISR_H_ */
