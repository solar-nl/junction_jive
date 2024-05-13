#include "memory.h"

void IWRAM_CODE fast_copy(uint16_t *dst, const uint16_t *src, int count)
{
     int n = (count + 7) >> 3;

     switch (count & 7)
     {
     case 0: do { *dst++ = *src++;
     case 7:      *dst++ = *src++;
     case 6:      *dst++ = *src++;
     case 5:      *dst++ = *src++;
     case 4:      *dst++ = *src++;
     case 3:      *dst++ = *src++;
     case 2:      *dst++ = *src++;
     case 1:      *dst++ = *src++;
     } while (--n > 0); 
     }
}

void IWRAM_CODE fast_set(uint16_t *dst, const uint16_t val, int count)
{
     int n = (count + 7) >> 3;

     switch (count & 7)
     {
     case 0: do { *dst++ = val;
     case 7:      *dst++ = val;
     case 6:      *dst++ = val;
     case 5:      *dst++ = val;
     case 4:      *dst++ = val;
     case 3:      *dst++ = val;
     case 2:      *dst++ = val;
     case 1:      *dst++ = val;
     } while (--n > 0); 
     }
}
