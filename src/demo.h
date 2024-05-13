#ifndef __DEMO_H
#define __DEMO_H

#include <stdint.h>

struct scene_t {
  void (*init)(void);
  void (*update)(uint16_t*, uint32_t, uint16_t);
  void (*destroy)(void);
};

void run_demo();

#endif /* __DEMO_H */