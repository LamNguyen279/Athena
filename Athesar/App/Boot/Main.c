/*
 * Main.c
 *
 *  Created on: Jul 30, 2023
 *      Author: lam nguyen
 *
               /\_/\   /\_/\
              ( o.o ) ( o.o )
               > ^ <   > ^ <
 *
 */
#include <stdio.h>

#include "vTask.h"
#include "vScheduler.h"
#include "vEvent.h"
#include "vAlarm.h"
#include "vIsr.h"

#include "SoAd.h"

volatile vTaskHandler_t *PostOsStartupTask;
volatile vTaskHandler_t *BswMainTask_1ms;
volatile vTaskHandler_t *Task_10ms;

volatile vAlarmHandler_t *BswMainAlarm_10ms;
volatile vAlarmHandler_t *Alarm_1ms;

void OsIdleHook()
{
  static int a;
  a++;
}

void OsTask_BswMain_1ms(void)
{
  SoAd_MainFunction();

  vTaskTerminate();
}

uint32 TestCaseIdx = -1;
uint32 TestSoConIdx = -1;
uint32 TestTxPduId = -1;
void OsTask_10ms(void)
{
  SoAd_SoConIdType testGetSoConId;
  Std_ReturnType ret = E_OK;
  PduInfoType pduInfo;

  char txBuff[100] = "This is from AtheSar\n";

  pduInfo.SduDataPtr = &txBuff[0];
  pduInfo.SduLength = sizeof("This is from AtheSar\n");

  switch(TestCaseIdx)
  {
  case 1:
    ret = SoAd_OpenSoCon(TestSoConIdx);
    TestCaseIdx = -1;
    break;
  case 2:
    ret = SoAd_CloseSoCon(TestSoConIdx, 0);
    TestCaseIdx = -1;
    break;
  case 3:
    ret = SoAd_GetSoConId(TestTxPduId, &testGetSoConId);
    TestCaseIdx = -1;
    break;
  case 4:
    ret = SoAd_IfTransmit(TestTxPduId, &pduInfo);
    TestCaseIdx = -1;
    break;
  case 5:
    ret = SoAd_TpTransmit(TestTxPduId, &pduInfo);
    TestCaseIdx = -1;
    break;
  default:
    break;
  }

  vTaskTerminate();
}

void OsTask_PostOsStartup(void)
{
  SoAd_Init(NULL);

  vTaskTerminate();
}

void Isr_KeyA_Pressed()
{
  TestCaseIdx = 4;
  TestSoConIdx = 0;
  TestTxPduId = 0;
}

void Isr_Key1_Pressed()
{
  TestCaseIdx = 5;
  TestSoConIdx = 1;
  TestTxPduId = 1;
}

void Isr_Key2_Pressed()
{
  TestCaseIdx = 1;
  TestSoConIdx = 0;
  TestTxPduId = 0;
}

void Isr_Key3_Pressed()
{
  TestCaseIdx = 2;
  TestSoConIdx = 0;
  TestTxPduId = 0;
}


int main()
{

  vTaskInit();
  vSchedulerInit();
  vEventInit();
  vIsrInit();

  PostOsStartupTask = vTaskCreate(OsTask_PostOsStartup, 255, VTASK_PREEMP_NON);
  BswMainTask_1ms = vTaskCreate(OsTask_BswMain_1ms, 10, VTASK_PREEMP_NON);
  Task_10ms = vTaskCreate(OsTask_10ms, 0, VTASK_PREEMP_FULL);

  vAlarmPar_t bswMainAlarmPar_10ms =
  {
      VALARM_AUTO,
      1,
      VALARM_ACTION_ACTIVATE_TASK,
      (void *)BswMainTask_1ms
  };
  vAlarmPar_t AlarmPar_10ms =
  {
      VALARM_AUTO,
      10,
      VALARM_ACTION_ACTIVATE_TASK,
      (void *)Task_10ms,
  };

  BswMainAlarm_10ms = vAlarmCreate(&bswMainAlarmPar_10ms);
  Alarm_1ms = vAlarmCreate(&AlarmPar_10ms);

  vIsrCreate(VISR_MASK_KEY_A, &Isr_KeyA_Pressed);
  vIsrCreate(VISR_MASK_KEY_1, &Isr_Key1_Pressed);
  vIsrCreate(VISR_MASK_KEY_2, &Isr_Key2_Pressed);
  vIsrCreate(VISR_MASK_KEY_3, &Isr_Key3_Pressed);

  vTaskActivate(PostOsStartupTask);

  vSchedulerStart();

  return 0;
}
