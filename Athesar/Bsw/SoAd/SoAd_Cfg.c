/*
 * SoAd_Cfg.c
 *
 *  Created on: Jul 31, 2023
 *      Author: ADMIN
 */

/* ***************************** [ INCLUDES  ] ****************************** */
#include "SoAd_Cfg.h"
#include "DoIP_Cbk.h"
#include "SoAd_Priv.h"
/* ***************************** [ MACROS    ] ****************************** */
/* ***************************** [ TYPES     ] ****************************** */
/* ***************************** [ DECLARES  ] ****************************** */
/* ***************************** [ DATAS     ] ****************************** */

const SoAd_CfgUpperFncTable_t SoAd_UpperFunctionTable[] =
{
    {
        &DoIP_SoAdIfRxIndication, //UpperIfRxIndication
        NULL_PTR, //UpperIfTriggerTransmit
        NULL_PTR, //UpperIfTxConfirmation
        &DoIP_SoAdTpStartOfReception, //UpperTpStartOfReception
        NULL_PTR, //UpperTpCopyRxData
        NULL_PTR, //UpperTpRxIndication
        NULL_PTR, //UpperTpCopyTxData
        NULL_PTR, //UpperTpTpTxConfirmation
        &DoIP_SoConModeChg, //UpperSoConModeChg
        NULL_PTR //LocalIpAddrAssignmentChg
    }
};

//SoAd_CfgSoConGrPar_t soconGr0Par = {
//    FALSE, //Server
//    VTCPIP_AF_INET,
//    VTCPIP_SOCK_DGRAM,
//    VTCPIP_IPPROTO_UDP,
//    "127.0.0.1",
//    12345,
//    NULL, //SoAdSocketIpAddrAssignmentChgNotification
//    NULL //SoAdSocketSoConModeChgNotification
//};
//
//SoAd_CfgSoConGrPar_t soconGr1Par = {
//    TRUE, //Server
//    VTCPIP_AF_INET,
//    VTCPIP_SOCK_STREAM,
//    VTCPIP_IPPROTO_TCP,
//    "127.0.0.1",
//    12345,
//    NULL, //SoAdSocketIpAddrAssignmentChgNotification
//    NULL //SoAdSocketSoConModeChgNotification
//};

const SoAd_CfgPduRoute_t SoAd_PduRouteArr[] =
{
    /* PduRoute 0 */
    {
        SOAD_COLLECT_LAST_IS_BEST, /* SoAdTxPduCollectionSemantics */
        1, /* SoAdTxPduRef */
        SOAD_UPPER_IF, /* SoAdTxUpperLayerType */
        SOAD_UPPER_DOIP, /* upper layer */
        0, /* SoAdPduRouteDestBase */
        1, /* SoAdPduRouteDestCtn */
    },
    /* PduRoute 1 */
    {
        SOAD_COLLECT_LAST_IS_BEST, /* SoAdTxPduCollectionSemantics */
        2, /* SoAdTxPduRef */
        SOAD_UPPER_TP, /* SoAdTxUpperLayerType */
        SOAD_UPPER_DOIP, /* upper layer */
        1, /* SoAdPduRouteDestBase */
        1, /* SoAdPduRouteDestCtn */
    },
    /* PduRoute 2 */
    {
        SOAD_COLLECT_LAST_IS_BEST, /* SoAdTxPduCollectionSemantics */
        3, /* SoAdTxPduRef */
        SOAD_UPPER_TP, /* SoAdTxUpperLayerType */
        SOAD_UPPER_DOIP, /* upper layer */
        2, /* SoAdPduRouteDestBase */
        1, /* SoAdPduRouteDestCtn */
    },
};
const uint32 SoAd_PduRouteArrSize = SOAD_GET_ARRAY_SIZE(SoAd_PduRouteArr);


const SoAd_CfgPduRouteDest_t SoAd_PduRouteDestArr[] =
{
    /* PduRouteDest 0 */
    {
        SOAD_INVALID_PDU_HEADER,/* SoAdTxPduHeaderId */
        SOAD_TRIGGER_ALWAYS,/* SoAdTxUdpTriggerMode */
        0,/* SoAdTxRoutingGroupBase */
        1,/* SoAdTxRoutingGroupCtn */
        SOAD_INVALID_SOCON_GROUP,/* SoAdTxSoConGrIdx */
        0,/* SoAdTxSoConIdx */
    },
    /* PduRouteDest 1 */
    {
        SOAD_INVALID_PDU_HEADER,/* SoAdTxPduHeaderId */
        SOAD_TRIGGER_ALWAYS,/* SoAdTxUdpTriggerMode */
        1,/* SoAdTxRoutingGroupBase */
        1,/* SoAdTxRoutingGroupCtn */
        SOAD_INVALID_SOCON_GROUP,/* SoAdTxSoConGrIdx */
        1,/* SoAdTxSoConIdx */
    },
    /* PduRouteDest 2 */
    {
        SOAD_INVALID_PDU_HEADER,/* SoAdTxPduHeaderId */
        SOAD_TRIGGER_ALWAYS,/* SoAdTxUdpTriggerMode */
        1,/* SoAdTxRoutingGroupBase */
        1,/* SoAdTxRoutingGroupCtn */
        SOAD_INVALID_SOCON_GROUP,/* SoAdTxSoConGrIdx */
        2,/* SoAdTxSoConIdx */
    }
};
const uint32 SoAd_PduRouteDestArrSize = SOAD_GET_ARRAY_SIZE(SoAd_PduRouteDestArr);


const SoAd_CfgRoutingGroup_t SoAd_RoutingGroupArr[] =
{
    /* RoutingGroup 0 */
    {
        SOAD_TRUE,/* IsEnabledAtInit */
        SOAD_TRUE,/* TxTriggerable */
    },
    /* RoutingGroup 1 */
    {
        SOAD_TRUE,/* IsEnabledAtInit */
        SOAD_TRUE,/* TxTriggerable */
    },
    /* RoutingGroup 2 */
    {
        SOAD_TRUE,/* IsEnabledAtInit */
        SOAD_TRUE,/* TxTriggerable */
    }
};
const uint32 SoAd_RoutingGroupArrSize = SOAD_GET_ARRAY_SIZE(SoAd_RoutingGroupArr);


const SoAd_CfgSoConGrp_t SoAd_SoConGrArr[] =
{
    {
        SOAD_UPPER_DOIP, /*SoAdUpperLayer */
        SOAD_FALSE,/* SoAdPduHeaderEnable */
        SOAD_FALSE,/* SoAdSocketAutomaticSoConSetup */
        5,/* SoAdSocketFramePriority */
        SOAD_TRUE,/* SoAdSocketIpAddrAssignmentChgNotification */
        10000,/* SoAdSocketLocalPort */
        SOAD_FALSE,/* SoAdSocketMsgAcceptanceFilterEnabled */
        SOAD_TRUE,/* SoAdSocketSoConModeChgNotification */
        0,/* SoAdSocketTpRxBufferMin */
        SOAD_TRUE, /* SoAdSocketTcpInitiate */
        VTCPIP_AF_INET, /* W32AfType */
        VTCPIP_SOCK_DGRAM, /* W32SocketType */
        VTCPIP_IPPROTO_UDP, /* W32ProtocolType */
        "127.0.0.1",/* W32LocalAddress[SOAD_IPV4_ADD_SIZE] */
    },
    {
        SOAD_UPPER_DOIP, /*SoAdUpperLayer */
        SOAD_FALSE,/* SoAdPduHeaderEnable */
        SOAD_FALSE,/* SoAdSocketAutomaticSoConSetup */
        5,/* SoAdSocketFramePriority */
        SOAD_TRUE,/* SoAdSocketIpAddrAssignmentChgNotification */
        10000,/* SoAdSocketLocalPort */
        SOAD_FALSE,/* SoAdSocketMsgAcceptanceFilterEnabled */
        SOAD_TRUE,/* SoAdSocketSoConModeChgNotification */
        0,/* SoAdSocketTpRxBufferMin */
        SOAD_TRUE, /* SoAdSocketTcpInitiate */
        VTCPIP_AF_INET, /* W32AfType */
        VTCPIP_SOCK_STREAM, /* W32SocketType */
        VTCPIP_IPPROTO_TCP, /* W32ProtocolType */
        "127.0.0.1",/* W32LocalAddress[SOAD_IPV4_ADD_SIZE] */
    },
    {
        SOAD_UPPER_DOIP, /*SoAdUpperLayer */
        SOAD_FALSE,/* SoAdPduHeaderEnable */
        SOAD_FALSE,/* SoAdSocketAutomaticSoConSetup */
        5,/* SoAdSocketFramePriority */
        SOAD_TRUE,/* SoAdSocketIpAddrAssignmentChgNotification */
        30000,/* SoAdSocketLocalPort */
        SOAD_FALSE,/* SoAdSocketMsgAcceptanceFilterEnabled */
        SOAD_TRUE,/* SoAdSocketSoConModeChgNotification */
        0,/* SoAdSocketTpRxBufferMin */
        SOAD_FALSE, /* SoAdSocketTcpInitiate */
        VTCPIP_AF_INET, /* W32AfType */
        VTCPIP_SOCK_STREAM, /* W32SocketType */
        VTCPIP_IPPROTO_TCP, /* W32ProtocolType */
        "127.0.0.1",/* W32LocalAddress[SOAD_IPV4_ADD_SIZE] */
    },
};
const uint32 SoAd_SoConGrpArrSize = SOAD_GET_ARRAY_SIZE(SoAd_SoConGrArr);


static const uint32 SoAdSoCon0_PduRouteDestList[]={0};
static const uint32 SoAdSoCon0_SocketRouteDestList[]={0};

static const uint32 SoAdSoCon1_PduRouteDestList[]={1};
static const uint32 SoAdSoCon1_SocketRouteDestList[]={1};

static const uint32 SoAdSoCon2_PduRouteDestList[]={2};
static const uint32 SoAdSoCon2_SocketRouteDestList[]={2};

const SoAd_CfgSoCon_t SoAd_SoConArr[] =
{
    /* SoAdSoCon0 */
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress */
        20000,/* SoAdSocketRemotePort */
        0, /* SoConGrIdx */
        &SoAdSoCon0_PduRouteDestList[0], /* PduRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon0_PduRouteDestList), /* PduRouteDestListSize */
        &SoAdSoCon0_SocketRouteDestList[0], /* SocketRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon0_SocketRouteDestList), /* SocketRouteDestListSize */
    },
    /* SoAdSoCon1 */
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE] */
        20001,/* SoAdSocketRemotePort */
        1, /* SoConGrIdx */
        &SoAdSoCon1_PduRouteDestList[0], /* PduRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon1_PduRouteDestList), /* PduRouteDestListSize */
        &SoAdSoCon1_SocketRouteDestList[0], /* SocketRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon1_SocketRouteDestList), /* SocketRouteDestListSize */
    },
    /* SoAdSoCon2 */
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE] */
        20002,/* SoAdSocketRemotePort */
        2, /* SoConGrIdx */
        &SoAdSoCon2_PduRouteDestList[0], /* PduRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon2_PduRouteDestList), /* PduRouteDestListSize */
        &SoAdSoCon2_SocketRouteDestList[0], /* SocketRouteDestList */
        SOAD_GET_ARRAY_SIZE(SoAdSoCon2_SocketRouteDestList), /* SocketRouteDestListSize */
    }
};
const uint32 SoAd_SoConArrSize = SOAD_GET_ARRAY_SIZE(SoAd_SoConArr);


const SoAd_CfgSocketRoute_t SoAd_SocketRouteArr[] =
{
    {
        SOAD_FALSE,/* SoAdRxPduHeaderId */
        1,/* SoAdRxPduId */
        SOAD_UPPER_IF,/* SoAdRxUpperLayerType */
        1, /* SoAdRxPduRef */
        0, /* SoAdRxRoutingGroupBase */
        1, /* SoAdRxRoutingGroupCtn */
        SOAD_UPPER_DOIP,
    },
    {
        SOAD_FALSE,/* SoAdRxPduHeaderId */
        2,/* SoAdRxPduId */
        SOAD_UPPER_TP,/* SoAdRxUpperLayerType */
        2, /* SoAdRxPduRef */
        0, /* SoAdRxRoutingGroupBase */
        1, /* SoAdRxRoutingGroupCtn */
        SOAD_UPPER_DOIP,
    },
    {
        SOAD_FALSE,/* SoAdRxPduHeaderId */
        3,/* SoAdRxPduId */
        SOAD_UPPER_TP,/* SoAdRxUpperLayerType */
        3, /* SoAdRxPduRef */
        0, /* SoAdRxRoutingGroupBase */
        1, /* SoAdRxRoutingGroupCtn */
        SOAD_UPPER_DOIP,
    }
};
const uint32 SoAd_CfgSocketRouteArrSize = SOAD_GET_ARRAY_SIZE(SoAd_SocketRouteArr);

/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
