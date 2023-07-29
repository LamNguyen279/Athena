
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
#define SOAD_MEAS_DROP_TCP ((SoAd_MeasurementIdxType)0x01)
#define SOAD_MEAS_DROP_UDP ((SoAd_MeasurementIdxType)0x02)
#define SOAD_MEAS_ALL ((SoAd_MeasurementIdxType)0x0xFF)

#define SOAD_CFG_NUM_SOAD_SOCON_GROUP             20
#define SOAD_CFG_NUM_SOAD_SOCKS         20
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

typedef struct SoAd_Config_s SoAd_ConfigType;

typedef enum _SoAd_W32SocketState_t
{
  VTCPIP_SOCK_STATE_INVALID,
  VTCPIP_SOCK_STATE_NEW,
  VTCPIP_SOCK_STATE_BIND,
  VTCPIP_SOCK_STATE_CONNECTED,
  VTCPIP_SOCK_STATE_LISTENING,
  VTCPIP_SOCK_STATE_ACCEPTED
} SoAd_W32SocketState_t;

typedef enum _vTcpIp_SocketType_t
{
  VTCPIP_SOCK_STREAM = 1,
  VTCPIP_SOCK_DGRAM = 2
} vTcpIp_SocketType_t;

typedef enum _vTcpIp_SocketAfType_t
{
  VTCPIP_AF_INET = 2,
  VTCPIP_AF_INET6 = 23
} vTcpIp_SocketAfType_t;

typedef enum _vTcpIp_SocketProtocolType_t
{
  VTCPIP_IPPROTO_TCP = 6,
  VTCPIP_IPPROTO_UDP = 17
} vTcpIp_SocketProtocolType_t;

typedef struct _vTcpIp_SocketPar_t
{
  vTcpIp_SocketAfType_t AfType;
  vTcpIp_SocketType_t SocketType;
  vTcpIp_SocketProtocolType_t Protocol;
  uint32_t Port;

} vTcpIp_SocketPar_t;

typedef enum _SoAdUpper_t
{
  SOAD_UPPER_INVALID = -1,
  SOAD_UPPER_DOIP
} SoAdUpper_t;

typedef struct _SoAdSock_t
{
  struct
  {
    HANDLE Hdl;
    DWORD Id;
  } W32Thread;
  SOCKET W32Sock;
  SoAd_W32SocketState_t W32SockState;
  char *RxBuff;
  uint32_t RxLength;
  void *GrAssigned;
  char  RemoteAddress[14];
  uint32 RemotePort;
  SoAdUpper_t Upper;
  //AUTOSAR
  uint32 RequestMask;
  SoAd_SoConIdType SoAdSoConId;
  SoAd_SoConModeType SoAdSoConState;
  PduIdType TxPduId;
  PduIdType UpperRxPduId;
  PduIdType UpperConfTxPduId;
} SoAdSock_t;

typedef struct _SoAdConGroup_t
{
  //static socket Group properties
  char  LocalAddress[14];
  int  AddressLength;
  vTcpIp_SocketAfType_t AfType;
  vTcpIp_SocketType_t SocketType;
  vTcpIp_SocketProtocolType_t ProtocolType;
  uint32 LocalPort;
  void *SoAdSocketIpAddrAssignmentChgNotification;
  void *SoAdSocketSoConModeChgNotification;
  // static socket Group protocol properties
  boolean IsServer;
  //W32 socket
  SOCKET W32SockListen;
} SoAdConGroupHandler_t;

typedef struct _SoAd_SoConGrPar_t
{
  boolean IsServer;
  vTcpIp_SocketAfType_t AfType;
  vTcpIp_SocketType_t SocketType;
  vTcpIp_SocketProtocolType_t Protocol;
  char  LocalAddress[14];
  uint32  LocalPort;
  void *SoAdSocketIpAddrAssignmentChgNotification;
  void *SoAdSocketSoConModeChgNotification;
} SoAd_SoConGrPar_t;
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */

/*  */
extern SoAdConGroupHandler_t *SoAd_CreateSoConGr(SoAd_SoConGrPar_t *SoConGrPar);
extern PduIdType SoAd_CreateSoCon(SoAdConGroupHandler_t *AssignedGr,   char  RemoteAddress[], uint32 RemotePort, PduIdType *UpperRxPduId, PduIdType *UpperTxPduId, SoAdUpper_t Upper);

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

#ifdef __cplusplus
}
#endif
#endif /* _SOAD_H */
