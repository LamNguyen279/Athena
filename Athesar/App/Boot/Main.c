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


#include "vTask.h"
#include "vScheduler.h"
#include "vEvent.h"
#include "vAlarm.h"

#include "SoAd.h"

volatile vTaskHandler_t *PostOsStartupTask;
volatile vTaskHandler_t *BswMainTask_10ms;
volatile vTaskHandler_t *Task_100ms;

volatile vAlarmHandler_t *BswMainAlarm_10ms;
volatile vAlarmHandler_t *Alarm_1ms;
volatile vAlarmHandler_t *AlarmTest;

void OsIdleHook()
{
  static int a;
  a++;
}

void OsTask_BswMain_10ms(void)
{
  SoAd_MainFunction();

  vTaskTerminate();
}

uint32 TestCaseIdx = -1;
uint32 TestSoConIdx = -1;
uint32 TestTxPduId = -1;
void OsTask_100ms(void)
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

void AlarmCallBackFncTest()
{
  static uint32 AlarmCallBackFncTestCtn;

  if(AlarmCallBackFncTestCtn == 10)
  {
    vAlarmStop(AlarmTest);
  }else
  {
    vAlarmStart(AlarmTest);
  }

  AlarmCallBackFncTestCtn++;
}

int main()
{

  vTaskInit();
  vSchedulerInit();
  vEventInit();

  PostOsStartupTask = vTaskCreate(OsTask_PostOsStartup, 255, VTASK_PREEMP_NON);
  BswMainTask_10ms = vTaskCreate(OsTask_BswMain_10ms, 10, VTASK_PREEMP_NON);
  Task_100ms = vTaskCreate(OsTask_100ms, 0, VTASK_PREEMP_FULL);

  vAlarmPar_t bswMainAlarmPar_10ms =
  {
      VALARM_AUTO,
      10,
      VALARM_ACTION_ACTIVATE_TASK,
      (void *)BswMainTask_10ms
  };
  vAlarmPar_t AlarmPar_100ms =
  {
      VALARM_AUTO,
      100,
      VALARM_ACTION_ACTIVATE_TASK,
      (void *)Task_100ms,
  };

  vAlarmPar_t AlarmTestPar =
  {
      VALARM_MANUAL,
      100,
      VALARM_ACTION_CALLBACK,
      (void *)AlarmCallBackFncTest,
  };

  BswMainAlarm_10ms = vAlarmCreate(&bswMainAlarmPar_10ms);
  Alarm_1ms = vAlarmCreate(&AlarmPar_100ms);

  AlarmTest = vAlarmCreate(&AlarmTestPar);
  vAlarmStart(AlarmTest);

  vTaskActivate(PostOsStartupTask);

  vSchedulerStart();

  return 0;
}
