#ifndef _DOIP_UTILS_H_
#define _DOIP_UTILS_H_

#include "Ath_Utils.h"

#define DoIP_MemCpy(dest, src, length) Ath_MemCpy(dest, src, length)
#define DoIP_MemSet(dest, src, length) Ath_MemSet(dest, src, length)

typedef Ath_TimeType DoIP_TimerType;

#define DoIP_IsTimerStopped(timer) Ath_IsTimerStopped(timer)
#define DoIP_IsTimerExpired(timer) Ath_IsTimerExpired(timer)
#define DoIP_StopTimer(timer) Ath_StopTimer(timer)
#define DoIP_StartTimer(timer, val) Ath_StartTimer(timer, val)
#define DoIP_ForceTimerExpired(timer) Ath_ForceTimerExpired(timer)
#define DoIP_TimerTick(timer) Ath_TimerTick(timer)

#define DoIP_CheckAnyFlagOn(val, flag) Ath_CheckAnyFlagOn(val, flag)
#define DoIP_CheckAllFlagOn(val, flag) Ath_CheckAllFlagOn(val, flag)
#define DoIP_SetFlag(val, flag) Ath_SetFlag(val, flag)
#define DoIP_ClearFlag(val, flag) Ath_ClearFlag(val, flag)


#endif