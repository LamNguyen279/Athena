#include "vTask.h"
#include "vScheduler.h"
#include "vEvent.h"

#include "SoAd.h"

vTaskHandler_t *PostOsStartupTask;
vTaskHandler_t *BswMainTask_10ms;
vTaskHandler_t *Task_1ms;

void OsIdleHook()
{
  static int a;
  a++;

}


void OsTask_BswMain_10ms(void)
{
  vEventMask_t OsTask_BswMain_10msMask;

  SoAd_OpenSoCon(0);

  SoAd_OpenSoCon(1);

  SoAd_OpenSoCon(2);

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


void OsTask_1ms(void)
{
  while(1)
  {
    vTaskTicksDelay(10);
    vEventSet( BswMainTask_10ms, OsEvent_BswMainFunction_10ms );
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


  PostOsStartupTask = vTaskCreate(OsTask_PostOsStartup, 255, VTASK_PREEMP_NON);

  BswMainTask_10ms = vTaskCreate(OsTask_BswMain_10ms, 10, VTASK_PREEMP_NON);

  Task_1ms = vTaskCreate(OsTask_1ms, 5, VTASK_PREEMP_FULL);

  vTaskActivate(PostOsStartupTask);

  vSchedulerStart();

  return 0;
}
