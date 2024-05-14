#include "demo.h"
#include "script.h"
#include "effects.h"

#define NUM_EFFECTS 8

uint16_t effect_frame = 0;
uint8_t current_effect = 0;

extern uint16_t *framebuffer;

const static struct scene_t scene_list[] = { 
  effect_junction,
  effect_torus,  
  effect_flowery,
  effect_kaleido,  
  effect_zwieber,    
  effect_credits,
  effect_greetings,  
  effect_end,
};

// 7.785 sec per overgang
const static int transitions[] = 
{
  0,          // junction
  225 * 4,    // torus
  225 * 6,    // flowery
  225 * 8,    // kaleidoscope
  225 * 10,   // zwieber
  225 * 12,   // credits
  225 * 16,   // scene
  225 * 20,   // greetings
  225 * 22    // while (1) {}
};

void reset_script()
{
  effect_frame = 0;
  current_effect = 0;
}

void start_script()
{
  reset_script();

  scene_list[current_effect].init();
}

void advance_script()
{
  effect_frame = 0;
  ++current_effect;

  current_effect %= NUM_EFFECTS;
}

void tick_script(uint32_t pulse)
{ 
  if (pulse >= transitions[current_effect + 1]) 
  {
    next_effect();
  }

  scene_list[current_effect].update(framebuffer, effect_frame++, pulse);  
}

void next_effect()
{
  scene_list[current_effect].destroy();

  advance_script();

  scene_list[current_effect].init();
}