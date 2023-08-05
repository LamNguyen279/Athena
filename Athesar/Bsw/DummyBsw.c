

#include "DoIP_Cbk.h"
#include "Stdio.h"

#ifndef UNUSED
#define UNUSED(variable) (void) variable
#endif

uint32 DoIP_SoAdIfRxIndicationCtn = 0;
void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  PduInfoType *PduInfoPtr)
{
  char buffer[256];

  memset(&buffer[0], 0, sizeof(buffer));
  memcpy(&buffer[0], PduInfoPtr->SduDataPtr, PduInfoPtr->SduLength);

  buffer[PduInfoPtr->SduLength] = "\0";

  DoIP_SoAdIfRxIndicationCtn++;

  printf("DoIP_SoAdIfRxIndication() %d \n", DoIP_SoAdIfRxIndicationCtn);
  printf("RxPduId = %d \n", RxPduId);
  printf("SduLength = %d \n", PduInfoPtr->SduLength);
  printf("SduDataPtr = %s \n", &buffer);
  fflush(stdout);
  return;
}

#define SOAD_SOCON_MODE_TO_STR(Mode) \
    ((Mode == SOAD_SOCON_OFFLINE) ? "SOAD_SOCON_OFFLINE" : \
        (Mode == SOAD_SOCON_ONLINE) ? "SOAD_SOCON_ONLINE" : "SOAD_SOCON_RECONNECT")

uint32 DoIP_SoConModeChgCtn = 0;
void DoIP_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode)
{
  DoIP_SoConModeChgCtn++;

  printf("DoIP_SoConModeChg() %d \n", DoIP_SoConModeChgCtn);
  printf("SoConId = %d \n", SoConId);
  printf("Mode = %s \n", SOAD_SOCON_MODE_TO_STR(Mode));
  fflush(stdout);

  if(SoConId == 2 && Mode == SOAD_SOCON_OFFLINE)
  {
    SoAd_OpenSoCon(2);
  }
}

uint32 DoIP_SoAdTpStartOfReceptionCtn = 0;
extern BufReq_ReturnType DoIP_SoAdTpStartOfReception(
    PduIdType id,
    const PduInfoType *info,
    PduLengthType TpSduLength,
    PduLengthType *bufferSizePtr)
{
  DoIP_SoAdTpStartOfReceptionCtn++;

  char buffer[256];

  memset(&buffer[0], 0, sizeof(buffer));
  memcpy(&buffer[0], info->SduDataPtr, info->SduLength);

  buffer[info->SduLength] = "\0";

  printf("DoIP_SoAdTpStartOfReception() %d \n", DoIP_SoAdTpStartOfReceptionCtn);
  printf("RxPduId = %d \n", id);
  printf("SduLength = %d \n", info->SduLength);
  printf("SduDataPtr = %s \n", &buffer);

  fflush(stdout);

  if(info->SduLength == 0)
  {
    *bufferSizePtr = (rand() % 10);

    return BUFREQ_OK;
  }
}

uint8 DoIP_SoAdTpCopyRxDataBuff[1024];
uint32 DoIP_SoAdTpCopyRxDataCopyIdx = 0;
BufReq_ReturnType DoIP_SoAdTpCopyRxData(
  PduIdType id,
  const PduInfoType *info,
  PduLengthType *bufferSizePtr)
{

  memcpy(&DoIP_SoAdTpCopyRxDataBuff[DoIP_SoAdTpCopyRxDataCopyIdx], info->SduDataPtr, info->SduLength);

  if(info->SduLength == 0)
  {
    *bufferSizePtr = (rand() % 10);

    return BUFREQ_OK;
  }
}
