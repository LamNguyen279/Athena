
#ifndef _SOAD_H
#define _SOAD_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "ComStack_Types.h"
#include "TcpIp.h"
#include <winsock2.h>
#include <Ws2tcpip.h>

#ifdef __cplusplus
extern "C" {
#endif
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* @SWS_SoAd_00518 */
typedef uint16_t SoAd_SoConIdType;

/* @SWS_SoAd_00512 */
typedef enum
{
  SOAD_SOCON_ONLINE,
  SOAD_SOCON_RECONNECT,
  SOAD_SOCON_OFFLINE
} SoAd_SoConModeType;

/* @SWS_SoAd_00519 */
typedef uint16_t SoAd_RoutingGroupIdType;

/* @SWS_SoAd_91010 */
typedef uint8_t SoAd_MeasurementIdxType;

typedef struct _SoAd_ConfigType
{
  uint8 NoThing_0_0_;
} SoAd_ConfigType;

/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */

/* @SWS_SoAd_00093 */
void SoAd_Init(const SoAd_ConfigType *ConfigPtr);

/* @SWS_SoAd_00091 */
Std_ReturnType SoAd_IfTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr);

/* @SWS_SoAd_00656 */
Std_ReturnType SoAd_IfRoutingGroupTransmit(SoAd_RoutingGroupIdType id);

/* @SWS_SoAd_00711 */
Std_ReturnType SoAd_IfSpecificRoutingGroupTransmit(SoAd_RoutingGroupIdType id,
                                                   SoAd_SoConIdType SoConId);

/* @SWS_SoAd_00105 */
Std_ReturnType SoAd_TpTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr);

/* @SWS_SoAd_00522 */
Std_ReturnType SoAd_TpCancelTransmit(PduIdType TxPduId);

/* @SWS_SoAd_00521 */
Std_ReturnType SoAd_TpCancelReceive(PduIdType RxPduId);

/* @SWS_SoAd_00509 */
Std_ReturnType SoAd_GetSoConId(PduIdType TxPduId, SoAd_SoConIdType *SoConIdPtr);

/* @SWS_SoAd_00510 */
Std_ReturnType SoAd_OpenSoCon(SoAd_SoConIdType SoConId);

/* @SWS_SoAd_00511 */
Std_ReturnType SoAd_CloseSoCon(SoAd_SoConIdType SoConId, boolean abort);

/* @SWS_SoAd_91001 */
void SoAd_GetSoConMode(SoAd_SoConIdType SoConId, SoAd_SoConModeType *ModePtr);

/* @SWS_SoAd_00520 */
Std_ReturnType SoAd_RequestIpAddrAssignment(SoAd_SoConIdType SoConId,
                                            TcpIp_IpAddrAssignmentType Type,
                                            const TcpIp_SockAddrType *LocalIpAddrPtr,
                                            uint8_t Netmask,
                                            const TcpIp_SockAddrType *DefaultRouterPtr);

/* @SWS_SoAd_00536 */
Std_ReturnType SoAd_ReleaseIpAddrAssignment(SoAd_SoConIdType SoConId);

/* @SWS_SoAd_00506 */
Std_ReturnType SoAd_GetLocalAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *LocalAddrPtr,
                                 uint8_t *NetmaskPtr, TcpIp_SockAddrType *DefaultRouterPtr);

/* @SWS_SoAd_00507 */
Std_ReturnType SoAd_GetPhysAddr(SoAd_SoConIdType SoConId, uint8_t *PhysAddrPtr);

/* @SWS_SoAd_00655 */
Std_ReturnType SoAd_GetRemoteAddr(SoAd_SoConIdType SoConId, TcpIp_SockAddrType *IpAddrPtr);

/* @SWS_SoAd_00515 */
Std_ReturnType SoAd_SetRemoteAddr(SoAd_SoConIdType SoConId,
                                  const TcpIp_SockAddrType *RemoteAddrPtr);

/* @SWS_SoAd_00121 */
void SoAd_MainFunction(void);

void SoAd_LocalIpAddrAssignmentChg(TcpIp_LocalAddrIdType IpAddrId,  TcpIp_IpAddrStateType State);

#ifdef __cplusplus
}
#endif
#endif /* _SOAD_H */
