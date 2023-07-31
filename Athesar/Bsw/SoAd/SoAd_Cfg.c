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

SoAd_CfgUpperFncTable_t SoAd_UpperFunctionTable[] =
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

SoAd_CfgSoConGrPar_t soconGr0Par = {
    FALSE, //Server
    VTCPIP_AF_INET,
    VTCPIP_SOCK_DGRAM,
    VTCPIP_IPPROTO_UDP,
    "127.0.0.1",
    12345,
    NULL, //SoAdSocketIpAddrAssignmentChgNotification
    NULL //SoAdSocketSoConModeChgNotification
};

SoAd_CfgSoConGrPar_t soconGr1Par = {
    TRUE, //Server
    VTCPIP_AF_INET,
    VTCPIP_SOCK_STREAM,
    VTCPIP_IPPROTO_TCP,
    "127.0.0.1",
    12345,
    NULL, //SoAdSocketIpAddrAssignmentChgNotification
    NULL //SoAdSocketSoConModeChgNotification
};

const SoAd_CfgPduRoute_t SoAd_PduRouteArr[] =
{
    /* PduRoute 0 */
    {
        SOAD_COLLECT_LAST_IS_BEST, /* SoAdTxPduCollectionSemantics */
        1, /* SoAdTxPduRef */
        SOAD_UPPER_IF, /* SoAdTxUpperLayerType */
        0, /* SoAdPduRouteDestBase */
        1, /* SoAdPduRouteDestCtn */
    },
    /* PduRoute 1 */
    {
        SOAD_COLLECT_LAST_IS_BEST, /* SoAdTxPduCollectionSemantics */
        2, /* SoAdTxPduRef */
        SOAD_UPPER_TP, /* SoAdTxUpperLayerType */
        1, /* SoAdPduRouteDestBase */
        1, /* SoAdPduRouteDestCtn */
    },
};
const uint32 SoAd_PduRouteArrSize = SOAD_GET_ARRAY_SIZE(SoAd_PduRouteArr);


const SoAd_CfgPduRouteDest_t SoAd_PduRouteDestArr[] =
{
    /* PduRouteDest 0 */
    {
        0,/* SoAdPduRouteIdx */
        SOAD_INVALID_PDU_HEADER,/* SoAdTxPduHeaderId */
        SOAD_TRIGGER_ALWAYS,/* SoAdTxUdpTriggerMode */
        0,/* SoAdTxRoutingGroupBase */
        1,/* SoAdTxRoutingGroupCtn */
        0,/* SoAdTxSoConIdBase */
        1,/* SoAdTxSoConIdCtn */
    },
    /* PduRouteDest 1 */
    {
        0,/* SoAdPduRouteIdx */
        SOAD_INVALID_PDU_HEADER,/* SoAdTxPduHeaderId */
        SOAD_TRIGGER_ALWAYS,/* SoAdTxUdpTriggerMode */
        0,/* SoAdTxRoutingGroupBase */
        1,/* SoAdTxRoutingGroupCtn */
        1,/* SoAdTxSoConIdBase */
        2,/* SoAdTxSoConIdCtn */
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
    }
};
const uint32 SoAd_RoutingGroupArrSize = SOAD_GET_ARRAY_SIZE(SoAd_RoutingGroupArr);


const SoAd_CfgSoConGrp_t SoAd_SoConGrpArr[] =
{
    {
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
};
const uint32 SoAd_SoConGrpArrSize = SOAD_GET_ARRAY_SIZE(SoAd_SoConGrpArr);


const SoAd_CfgSoCon_t SoAd_SoConArr[] =
{
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE] */
        20000,/* SoAdSocketRemotePort */
        0, /* SoConGrIdx */
        0, /* SocketRouteIdx */
    },
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE] */
        20001,/* SoAdSocketRemotePort */
        1, /* SoConGrIdx */
        0, /* SocketRouteIdx */
    },
    {
        "127.0.0.2", /* SoAdSocketRemoteIpAddress[SOAD_IPV4_ADD_SIZE] */
        20002,/* SoAdSocketRemotePort */
        1, /* SoConGrIdx */
        0, /* SocketRouteIdx */
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
    },
    {
        SOAD_FALSE,/* SoAdRxPduHeaderId */
        2,/* SoAdRxPduId */
        SOAD_UPPER_TP,/* SoAdRxUpperLayerType */
        2, /* SoAdRxPduRef */
        0, /* SoAdRxRoutingGroupBase */
        1, /* SoAdRxRoutingGroupCtn */
    },
    {
        SOAD_FALSE,/* SoAdRxPduHeaderId */
        3,/* SoAdRxPduId */
        SOAD_UPPER_TP,/* SoAdRxUpperLayerType */
        3, /* SoAdRxPduRef */
        0, /* SoAdRxRoutingGroupBase */
        1, /* SoAdRxRoutingGroupCtn */
    }
};
const uint32 SoAd_CfgSocketRouteArrSize = SOAD_GET_ARRAY_SIZE(SoAd_SocketRouteArr);

/* ***************************** [ LOCALS    ] ****************************** */
/* ***************************** [ FUNCTIONS ] ****************************** */
