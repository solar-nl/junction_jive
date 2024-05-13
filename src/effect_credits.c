#include "sprites.h"
#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

uint8_t credits_state = 0; 

void effect_credits_init()
{
  load_model(ster_model, 126, MODEL_ENVMAPPED | MODEL_SORTFACES | MODEL_NOCLIP, 1, 0);
  finalize_mesh(126);

  current_model.position.z = -500;
  current_model.euler_angles.y = 8;

  set_palette(tex_skydomePal);

  clear_sprites();

  fast_set((unsigned short*)0x5000200, 0, 256);

  set_sprite(0, 120-32-32, 80-32-32, 64, 0, 0, ravity_test_perlin1Tiles);
  set_sprite(1, 120+32-32, 80-32-32, 64, 0, 0, ravity_test_perlin2Tiles);
  set_sprite(2, 120-32-32, 80+32-32, 64, 0, 0, ravity_test_perlin3Tiles);
  set_sprite(3, 120+32-32, 80+32-32, 64, 0, 0, ravity_test_perlin4Tiles);

  commit_sprites();

  set_sprites_transparency(16,16);    
}

void effect_credits_destroy()
{
}

const int speed = 16;
const uint16_t gradient_size = 32;
const uint16_t step_size = 32 / gradient_size;
uint16_t color_white = 0x7FFF;
uint16_t color_black = 0x0000;
uint16_t color;
uint8_t value;

void animate_palette_forward(uint8_t normal, uint16_t delay, uint32_t frame) {
  uint16_t fill_indices = min(255, (frame - delay) * speed);

  if (frame >= delay) {
    uint16_t gradient_index = 0;
    for (uint16_t i = 0; i < fill_indices; ++i) {
      if (i < fill_indices - gradient_size || (normal && (255 - gradient_size) < i) || (!normal && gradient_size <= i)) {
        ((uint16_t *)0x5000200)[i] = color_white;
      }
      else {
        value = 31 - step_size * gradient_index;
        color = (value << 10) | (value << 5) | (value);
        ((uint16_t *)0x5000200)[i] = color;

        ++gradient_index;
      }
    }
  }
}

void animate_palette_backward(uint8_t normal, uint16_t delay, uint32_t frame) {
  uint16_t fill_indices = min(255, (frame - delay) * speed);

  if (frame >= delay) {
    uint16_t gradient_index = 0;
    for (uint16_t i = 0; i < fill_indices; ++i) {
      if (i < fill_indices - gradient_size || (normal && gradient_size <= i) || (!normal && (255 - gradient_size) < i)) {
        ((uint16_t *)0x5000200)[255 - i] = 0x0000;
      }
      else {
        value = 31 - step_size * gradient_index;
        color = (value << 10) | (value << 5) | (value);
        ((uint16_t *)0x5000200)[255 - i] = color;

        ++gradient_index;
      }
    }
  }
}

void effect_credits_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    current_model.euler_angles.z = (frame << 1) - 80;

    if (frame < 32) {
        int fade_to_black = min(frame << 3, 255);
        fade_palette((unsigned short*)0x5000000, tex_skydomePal, 0, 0, 0, 255 - fade_to_black);
    }
    else {
        exposure_palette((uint16_t*)0x5000000, tex_skydomePal, 0 + abs(sin((frame - 64) << 2) >> 2));
    }

    fast_copy(target, achtergrond_zwierBitmap, VIEWPORT_WIDTH * VIEWPORT_HEIGHT >> 1);
    render(&current_model);

    if (frame > 32 && frame < 65) { 
      animate_palette_forward(1, 32, frame);
    }

    if (frame > 75 && credits_state == 0) {
      clear_sprites();

      fast_set((unsigned short*)0x5000200, 0, 256);

      set_sprite(0, 120-32-32, 80-32-32, 64, 0, 0, coat_test_perslin1Tiles);
      set_sprite(1, 120+32-32, 80-32-32, 64, 0, 0, coat_test_perslin2Tiles);
      set_sprite(2, 120-32-32, 80+32-32, 64, 0, 0, coat_test_perslin3Tiles);
      set_sprite(3, 120+32-32, 80+32-32, 64, 0, 0, coat_test_perslin4Tiles);

      commit_sprites();      

      credits_state = 1;
    }

    if (frame > 75 + 32 && credits_state == 1) { 
      animate_palette_forward(1, 75 + 32, frame);
    }

    if (frame > 75 + 32 + 43 && credits_state == 1) {
      fast_set((unsigned short*)0x5000200, 0, 256);
      credits_state = 3;
    }
}