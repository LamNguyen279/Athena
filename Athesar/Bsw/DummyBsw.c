

#include "DoIP_Cbk.h"
#include "Stdio.h"

#ifndef UNUSED
#define UNUSED(variable) (void) variable
#endif

#define DOIP_RANDOM_BUFF_SIZE 10

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

  printf("----------------------------------------------------------------\n");
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

  printf("----------------------------------------------------------------\n");
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

  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdTpStartOfReception() %d \n", DoIP_SoAdTpStartOfReceptionCtn);
  printf("RxPduId = %d \n", id);
  printf("info->SduLength = %d \n", info->SduLength);

  if(info->SduDataPtr != NULL)
  {
    printf("info->SduDataPtr = %s \n", info->SduDataPtr);

  }else
  {
    printf("info->SduDataPtr = %s \n", "NULL_PTR");
  }

  printf("TpSduLength = %d \n", TpSduLength);

  fflush(stdout);

  if(info->SduLength == 0)
  {
    *bufferSizePtr = (rand() % DOIP_RANDOM_BUFF_SIZE);

    return BUFREQ_OK;
  }
}

uint8 DoIP_SoAdTpCopyRxDataBuff[10][256];
uint32 DoIP_SoAdTpCopyRxDataCopyIdx = 0;
uint32 DoIP_SoAdTpCopyRxDataCtn = 0;
BufReq_ReturnType DoIP_SoAdTpCopyRxData(
  PduIdType id,
  const PduInfoType *info,
  PduLengthType *bufferSizePtr)
{

  DoIP_SoAdTpCopyRxDataCtn++;

  memcpy(&DoIP_SoAdTpCopyRxDataBuff[id][DoIP_SoAdTpCopyRxDataCopyIdx], info->SduDataPtr, info->SduLength);

  DoIP_SoAdTpCopyRxDataCopyIdx += info->SduLength;

  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdTpCopyRxData() %d \n", DoIP_SoAdTpCopyRxDataCtn);
  printf("RxPduId = %d \n", id);
  printf("info->SduLength = %d \n", info->SduLength);

//  if(info->SduDataPtr != NULL)
//  {
//    printf("info->SduDataPtr = %s \n", info->SduDataPtr);
//
//  }else
//  {
//    printf("info->SduDataPtr = %s \n", "NULL_PTR");
//  }

  fflush(stdout);

  *bufferSizePtr = (rand() % DOIP_RANDOM_BUFF_SIZE);

  return BUFREQ_OK;
}

uint32 DoIP_SoAdTpRxIndicationCtn = 0;
void DoIP_SoAdTpRxIndication(
  PduIdType id,
  Std_ReturnType result)
{
  DoIP_SoAdTpRxIndicationCtn++;

  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdTpRxIndication() %d \n", DoIP_SoAdTpRxIndicationCtn);
  printf("id = %d \n", id);
  printf("result = %d \n", result);

  printf("CopiedBuffer: %s \n", &DoIP_SoAdTpCopyRxDataBuff[id][0]);

  memset(&DoIP_SoAdTpCopyRxDataBuff[id][0], 0, sizeof(DoIP_SoAdTpCopyRxDataBuff[0]));
  DoIP_SoAdTpCopyRxDataCopyIdx = 0;

  fflush(stdout);
}
