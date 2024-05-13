#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <gba_base.h>

#if 0
#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#endif

void fast_copy(uint16_t *dst, const uint16_t *src, int count);
void fast_set(uint16_t *dst, const uint16_t val, int count);

#endif /* __MEMORY_H */