#ifndef _ATH_UTILS_H_
#define _ATH_UTILS_H_

#include "Std_Types.h"
#include "Ath_Types.h"

#define ATHENA_VENDOR_ID ((uint16) 0x5555)


#define ATH_TIMER_VAL_STOPPED ((Ath_TimeType) 0)
#define ATH_TIMER_VAL_EXPIRED ((Ath_TimeType) 1)

#define Ath_IsTimerStopped(timer) (ATH_TIMER_VAL_STOPPED == timer)
#define Ath_IsTimerExpired(timer) (ATH_TIMER_VAL_EXPIRED == timer)
#define Ath_StopTimer(timer) (timer = ATH_TIMER_VAL_STOPPED)
#define Ath_StartTimer(timer, val) (timer = (ATH_TIMER_VAL_EXPIRED + val))
#define Ath_ForceTimerExpired(timer) (timer = ATH_TIMER_VAL_EXPIRED)
#define Ath_TimerTick(timer) \
          if(timer > ATH_TIMER_VAL_EXPIRED)\
          {\
            timer--;\
          }

#define Ath_CheckAnyFlagOn(val, flag) ((val & flag) != 0)
#define Ath_CheckAllFlagOn(val, flag) ((val & flag) == flag)
#define Ath_SetFlag(val, flag) (val |= flag)
#define Ath_ClearFlag(val, flag) (val &= ~flag)


extern void Ath_MemCpy(uint8* dest, uint8* src, uint32 length);
extern void Ath_MemSet(uint8* dest, uint8 val, uint32 length);


#endif