#include "Ath_Utils.h"

void Ath_MemCpy(uint8* dest, uint8* src, uint32 length)
{
  uint32 idx;
  for(idx = 0; idx < length; idx++)
  {
    dest[idx] = src[idx];
  }
}

void Ath_MemSet(uint8* dest, uint8 val, uint32 length)
{
  uint32 idx;
  for(idx = 0; idx < length; idx++)
  {
    dest[idx] = val;
  }
}