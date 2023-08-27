#include "Std_Types.h"
#include "ComStack_Types.h"
#include "DoIP_Cfg.h"
#include "SoAd.h"
#include "DoIP_IntFunc.h"
#include "DoIP_IntTypes.h"
#include "DoIP_PubTypes.h"
#include "DoIP_Globals.h"


uint16 FindConBySoConId(
  SoAd_SoConIdType SoConId,
  DoIP_ConType* ConType)
{
  uint16 conIdx;
  uint16 retVal;

  retVal = DOIP_INVALID_CON_IDX;

  conIdx = 0;
  while ((retVal == DOIP_INVALID_CON_IDX) && (conIdx < DOIP_NUM_TCP_CON))
  {
    if(DoIP_TcpConData[conIdx].CfgTcpConPtr->SoConId == SoConId)
    {
      retVal = conIdx;
      *ConType = DOIP_CON_TYPE_TCP;
    }
    else
    {
      conIdx++;
    }
  }

  conIdx = 0;
  while ((retVal == DOIP_INVALID_CON_IDX) && (conIdx < DOIP_NUM_UDP_CON))
  {
    if(DoIP_UdpConData[conIdx].CfgUdpConPtr->SoConId == SoConId)
    {
      retVal = conIdx;
      *ConType = DOIP_CON_TYPE_UDP;
    }
    else
    {
      conIdx++;
    }
  }

  conIdx = 0;
  while ((retVal == DOIP_INVALID_CON_IDX) && (conIdx < DOIP_NUM_ANN_CON))
  {
    if(DoIP_AnnConData[conIdx].CfgAnnConPtr->SoConId == SoConId)
    {
      retVal = conIdx;
      *ConType = DOIP_CON_TYPE_ANN;
    }
    else
    {
      conIdx++;
    }
  }

  return retVal;
}

void DoIP_SwitchLineActivationActive(uint8 InterfaceId)
{
  uint16 conIdx;
  uint16 conEndIdx;
  DoIP_InterfaceCfgType* CfgInterfacePtr;

  CfgInterfacePtr = DoIP_GetInterfaceCfg(InterfaceId);

  conIdx = CfgInterfacePtr->TcpConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumTcpCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_TcpConData[conIdx].CfgTcpConPtr->RequestAddressAssignment)
    {
      SoAd_RequestIpAddrAssignment(DoIP_TcpConData[conIdx].CfgTcpConPtr->SoConId,
                                   TCPIP_IPADDR_ASSIGNMENT_ALL, NULL_PTR, DOIP_DEFAULT_NETMASK, NULL_PTR);
    }

    SoAd_OpenSoCon(DoIP_TcpConData[conIdx].CfgTcpConPtr->SoConId);
  }

  conIdx = CfgInterfacePtr->UdpConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumUdpCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_UdpConData[conIdx].CfgUdpConPtr->RequestAddressAssignment)
    {
      SoAd_RequestIpAddrAssignment(DoIP_UdpConData[conIdx].CfgUdpConPtr->SoConId,
                                   TCPIP_IPADDR_ASSIGNMENT_ALL, NULL_PTR, DOIP_DEFAULT_NETMASK, NULL_PTR);
    }

    SoAd_OpenSoCon(DoIP_UdpConData[conIdx].CfgUdpConPtr->SoConId);
  }

  conIdx = CfgInterfacePtr->AnnConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumAnnCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_AnnConData[conIdx].CfgAnnConPtr->RequestAddressAssignment)
    {
      SoAd_RequestIpAddrAssignment(DoIP_AnnConData[conIdx].CfgAnnConPtr->SoConId,
                                   TCPIP_IPADDR_ASSIGNMENT_ALL, NULL_PTR, DOIP_DEFAULT_NETMASK, NULL_PTR);
    }

    SoAd_OpenSoCon(DoIP_AnnConData[conIdx].CfgAnnConPtr->SoConId);
  }
}

void DoIP_SwitchLineActivationInactive(uint8 InterfaceId)
{
  uint16 conIdx;
  uint16 conEndIdx;
  DoIP_InterfaceCfgType* CfgInterfacePtr;

  CfgInterfacePtr = DoIP_GetInterfaceCfg(InterfaceId);

  conIdx = CfgInterfacePtr->TcpConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumTcpCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_TcpConData[conIdx].CfgTcpConPtr->RequestAddressAssignment)
    {
      SoAd_ReleaseIpAddrAssignment(DoIP_TcpConData[conIdx].CfgTcpConPtr->SoConId);
    }

    SoAd_CloseSoCon(DoIP_TcpConData[conIdx].CfgTcpConPtr->SoConId, FALSE);
  }

  conIdx = CfgInterfacePtr->UdpConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumUdpCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_UdpConData[conIdx].CfgUdpConPtr->RequestAddressAssignment)
    {
      SoAd_ReleaseIpAddrAssignment(DoIP_UdpConData[conIdx].CfgUdpConPtr->SoConId);
    }

    SoAd_CloseSoCon(DoIP_UdpConData[conIdx].CfgUdpConPtr->SoConId, FALSE);
  }

  conIdx = CfgInterfacePtr->AnnConStartIdx;
  conEndIdx = conIdx + CfgInterfacePtr->NumAnnCon;
  for(; conIdx < conEndIdx; conIdx++)
  {
    if(TRUE == DoIP_AnnConData[conIdx].CfgAnnConPtr->RequestAddressAssignment)
    {
      SoAd_ReleaseIpAddrAssignment(DoIP_AnnConData[conIdx].CfgAnnConPtr->SoConId);
    }

    SoAd_CloseSoCon(DoIP_AnnConData[conIdx].CfgAnnConPtr->SoConId, FALSE);
  }
}