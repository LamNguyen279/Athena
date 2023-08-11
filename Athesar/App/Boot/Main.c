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

#include "SoAd.h"

volatile vTaskHandler_t *PostOsStartupTask;
volatile vTaskHandler_t *BswMainTask_10ms;
volatile vTaskHandler_t *Task_1ms;

void OsIdleHook()
{
  static int a;
  a++;

}


void OsTask_BswMain_10ms(void)
{
  vEventMask_t OsTask_BswMain_10msMask;

  while(1)
  {
    vEventWait( OsEvent_BswMainFunction_10ms );
    vEventGet( BswMainTask_10ms, &OsTask_BswMain_10msMask );

    if((OsTask_BswMain_10msMask & OsEvent_BswMainFunction_10ms) == OsEvent_BswMainFunction_10ms)
    {
      vEventClear(OsEvent_BswMainFunction_10ms);

      SoAd_MainFunction();
    }
  }
}

uint32 TestCaseIdx = -1;
uint32 TestSoConIdx = -1;
uint32 TestTxPduId = -1;

uint32 OsTask_1msCtn = 0;
void OsTask_1ms(void)
{
  SoAd_SoConIdType testGetSoConId;
  Std_ReturnType ret = E_OK;
  PduInfoType pduInfo;

  char txBuff[100] = "This is from AtheSar\n";

  pduInfo.SduDataPtr = &txBuff[0];
  pduInfo.SduLength = sizeof("This is from AtheSar\n");

  while(1)
  {
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

    OsTask_1msCtn++;

    if(OsTask_1msCtn == 10)
    {
      OsTask_1msCtn = 0;

      vEventSet( BswMainTask_10ms, OsEvent_BswMainFunction_10ms );
    }

    vTaskTicksDelay(1);
  }
}

void OsTask_PostOsStartup(void)
{
  SoAd_Init(NULL);

  vTaskActivate(Task_1ms);
  vTaskActivate(BswMainTask_10ms);

  vTaskTerminate();
}

int main()
{

  vTaskInit();
  vSchedulerInit();
  vEventInit();

  PostOsStartupTask = vTaskCreate(OsTask_PostOsStartup, 255, VTASK_PREEMP_NON);

  BswMainTask_10ms = vTaskCreate(OsTask_BswMain_10ms, 10, VTASK_PREEMP_FULL);

  Task_1ms = vTaskCreate(OsTask_1ms, 0, VTASK_PREEMP_FULL);

  vTaskActivate(PostOsStartupTask);

  vSchedulerStart();

  return 0;
}
