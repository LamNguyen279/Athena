#include "vTask.h"
#include "vScheduler.h"
#include "vEvent.h"
#include "vOs_Cfg.h"

#include <sys\timeb.h>

vTaskHandler_t *Task0hdl, *Task1hdl, *Task2hdl, *Task3hdl;

void OsIdleHook()
{
  static int a;
  a++;

  if(a % 500 == 0)
  {
    vTaskActivate(Task2hdl); //A
    vEventSet(Task3hdl, vOsEvent0);
    vEventSet(Task3hdl, vOsEvent1);
  }

}

void task0(void)
{
  uint32_t a = 1;


  vTaskTerminate();  //C
}

void task1(void)
{
  uint32_t a = 1;

  vTaskActivate(Task0hdl); //D


  vTaskTerminate(); //E
}

uint32_t b = 0;
void task2(void)
{
  uint32_t a = 1;

  vTaskActivate(Task1hdl); //F


  vTaskTerminate(); //G
}

void task3(void)
{
  uint32_t task3Ctn = 0;
  vEventMask_t task3EventMask;

  struct timeb start, end;
  int diff;

  while(1)
  {
    vEventWait( vOsEvent0 | vOsEvent1 );
    vEventGet( Task3hdl, &task3EventMask );

    if((task3EventMask & vOsEvent0) == vOsEvent0)
    {
      vEventClear(vOsEvent0);
      task3Ctn++;
    }

    if((task3EventMask & vOsEvent1) == vOsEvent1)
    {
      vEventClear(vOsEvent1);
      task3Ctn++;

      ftime(&end);
      diff = (int) (1000.0 * (end.time - start.time)
          + (end.millitm - start.millitm));

      printf("\nOperation took %u milliseconds\n", diff);
      fflush(stdout);
      ftime(&start);
    }
  }

  vTaskTerminate(); //I
}

int main()
{                    // entry, prio, Preempt policy
  Task0hdl = vTaskCreate(task0, 4, VTASK_PREEMP_FULL);
  Task1hdl = vTaskCreate(task1, 3, VTASK_PREEMP_NON);
  Task2hdl = vTaskCreate(task2, 1, VTASK_PREEMP_FULL);
  Task3hdl = vTaskCreate(task3, 2, VTASK_PREEMP_FULL);

  vTaskInit();
  vSchedulerInit();

  vTaskActivate(Task2hdl);
  vTaskActivate(Task3hdl);

  vSchedulerStart();

  return 0;
}
