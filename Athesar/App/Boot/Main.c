#include "vTask.h"
#include "vScheduler.h"
#include "vEvent.h"

#include "SoAd.h"

vTaskHandler_t *Task0hdl, *Task1hdl, *Task2hdl, *Task3hdl;

void OsIdleHook()
{
  static int a;
  a++;

  if(a % 10 == 0)
  {
    vTaskActivate(Task2hdl); //A
    vEventSet(Task3hdl, vOsEvent0);
    vEventSet(Task3hdl, vOsEvent1);
  }

}

void task0(void)
{
  uint32_t a = 1;

  for(a = 0; a < 10000; a++) //B
  {

  }

  vTaskTerminate();  //C
}

void task1(void)
{
  uint32_t a = 1;

  vTaskActivate(Task0hdl); //D

  for(a = 0; a < 100000; a++)
  {

  }

  vTaskTerminate(); //E
}

uint32_t b = 0;
void task2(void)
{
  vTaskActivate(Task1hdl); //F

  SoAd_MainFunction();

  vTaskTerminate(); //G
}

void task3(void)
{
  uint32_t task3Ctn = 0;
  vEventMask_t task3EventMask;


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
    }
  }
}

int main()
{

  vTaskInit();
  vSchedulerInit();
                        // entry, prio, Preempt policy
  Task0hdl = vTaskCreate(task0, 4, VTASK_PREEMP_FULL);
  Task1hdl = vTaskCreate(task1, 3, VTASK_PREEMP_NON);
  Task2hdl = vTaskCreate(task2, 1, VTASK_PREEMP_NON);
  Task3hdl = vTaskCreate(task3, 2, VTASK_PREEMP_NON);

  vTaskActivate(Task2hdl);
  vTaskActivate(Task3hdl);

  /* create socon group */
//  SoAdConGroupHandler_t *soConGr0 = SoAd_CreateSoConGr(&soconGr0Par);

  /* create Socon and assigned to Group soConGr0 */
//  PduIdType DoIpTcpConRxPduId = 2; //config
//  PduIdType DoIpTcpConTxPduId = 2; //config

//  PduIdType DoIpTcpConSoAdTxPduId = SoAd_CreateSoCon(soConGr0, "127.0.0.2", 12345, &DoIpTcpConRxPduId, &DoIpTcpConTxPduId, SOAD_UPPER_DOIP);

//  SoAd_SoConIdType DoIpTcpConSoConId;
//  SoAd_GetSoConId(DoIpTcpConSoAdTxPduId, &DoIpTcpConSoConId);
//
//  SoAd_OpenSoCon(DoIpTcpConSoConId);


  //test socon 2


//  SoAdConGroupHandler_t *soConGr1 = SoAd_CreateSoConGr(&soconGr1Par);
//
//  PduIdType UpperRxPduId1 = 3; //config
//  PduIdType UpperTxPduId1 = 3; //config
//
//  PduIdType UpperPduId1 = SoAd_CreateSoCon(soConGr1, "127.0.0.2", 23456, &UpperRxPduId1, &UpperTxPduId1, SOAD_UPPER_DOIP);
//
//  SoAd_SoConIdType UpperConSoConId1;
//  SoAd_GetSoConId(UpperPduId1, &UpperConSoConId1);
//
//  SoAd_OpenSoCon(UpperConSoConId1);


  //test socon 3

//  SoAdConGroupHandler_t *soConGr2 = SoAd_CreateSoConGr(&soconGr1Par);
//
//  PduIdType UpperRxPduId2 = 4; //config
//  PduIdType UpperTxPduId2 = 4; //config
//
//  PduIdType UpperPduId2 = SoAd_CreateSoCon(soConGr1, "127.0.0.2", 34567, &UpperRxPduId2, &UpperTxPduId2, SOAD_UPPER_DOIP);
//
//  SoAd_SoConIdType UpperConSoConId2;
//  SoAd_GetSoConId(UpperPduId2, &UpperConSoConId2);
//
//  SoAd_OpenSoCon(UpperConSoConId2);

  vSchedulerStart();

  return 0;
}
