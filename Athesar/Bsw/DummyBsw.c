

#include "DoIP_Cbk.h"
#include "Stdio.h"

#ifndef UNUSED
#define UNUSED(variable) (void) variable
#endif

uint32 DoIP_SoAdIfRxIndicationCtn;
void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  PduInfoType *PduInfoPtr)
{
  char buffer[50];

  memset(&buffer[0], 0, sizeof(buffer));
  memcpy(&buffer[0], PduInfoPtr->SduDataPtr, PduInfoPtr->SduLength);

  buffer[PduInfoPtr->SduLength] = "\0";

  DoIP_SoAdIfRxIndicationCtn++;

  printf("DoIP_SoAdIfRxIndication = %d \n", DoIP_SoAdIfRxIndicationCtn);
  printf("SduLength = %d \n", PduInfoPtr->SduLength);
  printf("PduInfoPtr->SduDataPtr = %s \n", &buffer);
  fflush(stdout);
  return;
}
