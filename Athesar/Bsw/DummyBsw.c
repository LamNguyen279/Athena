

#include "DoIP.h"
#include "Stdio.h"

#ifndef UNUSED
#define UNUSED(variable) (void) variable
#endif

#define DOIP_RANDOM_BUFF_SIZE 50

#define STD_RETURN_TYPE_AS_STRING(ret) ((ret == 0) ? ("E_OK") : ("E_NOT_OK"))

#ifdef USE_DUMMY_BSW

uint32 DoIP_SoAdIfRxIndicationCtn = 0;
void DoIP_SoAdIfRxIndication(
  PduIdType RxPduId,
  const PduInfoType* PduInfoPtr)
{
  DoIP_SoAdIfRxIndicationCtn++;

  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdIfRxIndication() %d \n", DoIP_SoAdIfRxIndicationCtn);
  printf("RxPduId = %d \n", RxPduId);
  printf("SduLength = %d \n", PduInfoPtr->SduLength);
  printf("SduDataPtr = %s \n", PduInfoPtr->SduDataPtr);
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
    srand(DoIP_SoAdTpStartOfReceptionCtn);
    *bufferSizePtr = (rand() % DOIP_RANDOM_BUFF_SIZE);
  }

  return BUFREQ_OK;
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

  fflush(stdout);

  srand(DoIP_SoAdTpCopyRxDataCtn);
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
  printf("result = %s \n", STD_RETURN_TYPE_AS_STRING(result));

  printf("Copied Size: %d\n", DoIP_SoAdTpCopyRxDataCopyIdx);

  DoIP_SoAdTpCopyRxDataBuff[id][DoIP_SoAdTpCopyRxDataCopyIdx] = '\0';
  printf("Copied Buffer:\n%s\n", &DoIP_SoAdTpCopyRxDataBuff[id][0]);

  memset(&DoIP_SoAdTpCopyRxDataBuff[id][0], 0, sizeof(DoIP_SoAdTpCopyRxDataBuff[0]));
  DoIP_SoAdTpCopyRxDataCopyIdx = 0;

  fflush(stdout);
}

#define IP_STATE_TO_STR(state) (state == 0) ? "TCPIP_IPADDR_STATE_ASSIGNED" : \
    (state == 1) ? "TCPIP_IPADDR_STATE_ONHOLD" : "TCPIP_IPADDR_STATE_UNASSIGNED"
void DoIP_LocalIpAddrAssignmentChg(
    SoAd_SoConIdType SoConId,
    TcpIp_IpAddrStateType State)
{
  printf("----------------------------------------------------------------\n");
  printf("DoIP_LocalIpAddrAssignmentChg() %d \n", DoIP_SoAdTpRxIndicationCtn);
  printf("SoConId = %d \n", SoConId);
  printf("State = %s \n", IP_STATE_TO_STR(State));

  fflush(stdout);
}


uint32 DoIP_SoAdIfTxConfirmationCtn = 0;
void DoIP_SoAdIfTxConfirmation(
    PduIdType TxPduId,
    Std_ReturnType result)
{
  DoIP_SoAdIfTxConfirmationCtn++;
  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdIfTxConfirmation() %d \n", DoIP_SoAdIfTxConfirmationCtn);
  printf("TxPduId = %d \n", TxPduId);
  printf("result = %s \n", STD_RETURN_TYPE_AS_STRING(result));

  fflush(stdout);
}

uint32 DoIP_SoAdTpTxConfirmationCtn = 0;
void DoIP_SoAdTpTxConfirmation(
    PduIdType TxPduId,
    Std_ReturnType result)
{
  DoIP_SoAdTpTxConfirmationCtn++;
  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdTpTxConfirmation() %d \n", DoIP_SoAdTpTxConfirmationCtn);
  printf("TxPduId = %d \n", TxPduId);
  printf("result = %s \n", STD_RETURN_TYPE_AS_STRING(result));

  fflush(stdout);
}


#define DOIP_SOADTPCOPY_STRING  "Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Hello World, This is from AtheSar !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

#define DOIP_SOADTPCOPY_STRING_SIZE sizeof(DOIP_SOADTPCOPY_STRING)

uint8 DoIP_SoAdTpCopyTxDataBuff[DOIP_SOADTPCOPY_STRING_SIZE];

uint32 DoIP_SoAdTpCopyTxDataBuffCopiedLength[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32 DoIP_SoAdTpCopyTxDataCtn = 0;
BufReq_ReturnType DoIP_SoAdTpCopyTxData(
    const PduIdType id,
    const PduInfoType *info,
    const RetryInfoType *retry,
    PduLengthType *availableDataPtr)
{
  DoIP_SoAdTpCopyTxDataCtn++;

  printf("----------------------------------------------------------------\n");
  printf("DoIP_SoAdTpCopyTxData() %d \n", DoIP_SoAdTpCopyTxDataCtn);
  printf("id = %d \n", id);
  printf("info->SduLength = %d \n", info->SduLength);
  fflush(stdout);

  if(info->SduLength == 0)
  {
    //ask size from SoAd
    srand(DoIP_SoAdTpCopyTxDataCtn);
    *availableDataPtr = (rand() % DOIP_RANDOM_BUFF_SIZE) + 1;
    memcpy(&DoIP_SoAdTpCopyTxDataBuff[0], DOIP_SOADTPCOPY_STRING, DOIP_SOADTPCOPY_STRING_SIZE);
  }else
  {
    //copying state
    memcpy(info->SduDataPtr,
    //copying on going
        &DoIP_SoAdTpCopyTxDataBuff[DoIP_SoAdTpCopyTxDataBuffCopiedLength[id]],
        info->SduLength);

    DoIP_SoAdTpCopyTxDataBuffCopiedLength[id] += info->SduLength;

    //create random buffer size for testing SOAD
    srand(DoIP_SoAdTpCopyTxDataCtn);
    *availableDataPtr = (rand() % DOIP_RANDOM_BUFF_SIZE) + 1;

    if((*availableDataPtr + DoIP_SoAdTpCopyTxDataBuffCopiedLength[id]) >= DOIP_SOADTPCOPY_STRING_SIZE)
    {
      *availableDataPtr = DOIP_SOADTPCOPY_STRING_SIZE - DoIP_SoAdTpCopyTxDataBuffCopiedLength[id];
    }

    if(DoIP_SoAdTpCopyTxDataBuffCopiedLength[id] == DOIP_SOADTPCOPY_STRING_SIZE)
    {
      //Copying done
      *availableDataPtr = 0;
      DoIP_SoAdTpCopyTxDataBuffCopiedLength[id] = 0;
    }
  }

  return BUFREQ_OK;
}

#endif
