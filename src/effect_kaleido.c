#include "effects.h"
#include "rendering.h"
#include "memory.h"
#include "fixedmath.h"
#include "sprites.h"
#include "assets.h"

#define KALEIDO_SIDES 9

extern triangle_t triangle_list[MAX_VERTICES];
extern vertex_t clip_buffer[12];

void IWRAM_CODE effect_kaleido_init()
{
  current_model.triangles = &triangle_list[0];
  for (uint16_t i = 0; i < KALEIDO_SIDES; ++i) {
    triangle_list[i].next = &triangle_list[i + 1];
  }

  current_model.triangles[KALEIDO_SIDES - 1].next = 0;

  current_model.position.x = 0;
  current_model.position.z = 0;
  current_model.position.y = 0;    
  current_model.euler_angles.x = 0;
  current_model.euler_angles.y = 0;
  current_model.euler_angles.z = 0;  

  set_overlay(0, 0, 0, 0, 0);
  set_sprites_transparency(10, 16);  

  //set_palette(huuuPal);
  set_sprite_palette(tex_godwatbotPal);
}

void effect_kaleido_destroy()
{
  clear_sprites();
  commit_sprites();
}

void IWRAM_CODE UpdateKaleidoscope(triangle_t f[], uint16_t sides, uint32_t time)
{
  const int angle = time;
  const int zoom = 16;
  const uint16_t scale = 150;

  vector2_t center;

  center.x = 32 + ((cos((time << 2))) >> 1);
  center.y = 64 + ((sin(((time << 1) - 85))) >> 1);

  vector2_t tc0, tc1, tc2;

  tc0.x = center.x + ((zoom * cos((angle - 85))) >> 6);
  tc0.y = center.y + ((zoom * sin((angle - 85))) >> 6);
  tc1.x = center.x + ((zoom * cos(angle)) >> 6);
  tc1.y = center.y + ((zoom * sin(angle)) >> 6);
  tc2.x = center.x + ((zoom * cos((angle + 85))) >> 6);
  tc2.y = center.y + ((zoom * sin((angle + 85))) >> 6);

  /* because it's only drawing half of the object some triangles need to be invalidated */
  for (uint16_t i = 0; i < 3; ++i) {
    f[0].vertices[i].position.x = 0;
    f[0].vertices[i].position.y = 0;
    f[0].vertices[i].position.z = 0;
    f[1].vertices[i].position.x = 0;
    f[1].vertices[i].position.y = 0;
    f[1].vertices[i].position.z = 0;    
    f[sides-1].vertices[i].position.x = 0;
    f[sides-1].vertices[i].position.y = 0;
    f[sides-1].vertices[i].position.z = 0;
    f[sides-2].vertices[i].position.x = 0;
    f[sides-2].vertices[i].position.y = 0;
    f[sides-2].vertices[i].position.z = 0;         
  }  

  float step;
  for (uint16_t i = 2; i < sides-2; ++i)
  {
    f[i].vertices[0].position.x = 0;
    f[i].vertices[0].position.y = 0;
    f[i].vertices[0].position.z = 1;

    step = (float)sides / i;
    f[i].vertices[1].position.x = scale * sin_lut[(int)(64 + (SINLUT_SIZE+1) / step) & SINLUT_SIZE] >> 8;
    f[i].vertices[1].position.y = scale * (2 * sin_lut[(int)((SINLUT_SIZE+1) / step) & SINLUT_SIZE]) >> 9;
    f[i].vertices[1].position.z = 1;

			
		step = (float)sides / (i + 1);
    f[i].vertices[2].position.x = scale * sin_lut[(int)(64 + (SINLUT_SIZE+1) / step) & SINLUT_SIZE] >> 8;
    f[i].vertices[2].position.y = scale * (2 * sin_lut[(int)((SINLUT_SIZE+1) / step) & SINLUT_SIZE]) >> 9;
    f[i].vertices[2].position.z = 1;

    f[i].vertices[0].coords.x = tc0.x;
    f[i].vertices[0].coords.y = tc0.y;
    f[i].vertices[1].coords.x = ((i & 1) == 0) ? tc1.x : tc2.x;
    f[i].vertices[1].coords.y = ((i & 1) == 0) ? tc1.y : tc2.y;
    f[i].vertices[2].coords.x = ((i & 1) == 0) ? tc2.x : tc1.x;
    f[i].vertices[2].coords.y = ((i & 1) == 0) ? tc2.y : tc1.y;    

    f[i].texture_index = 0;
  }
}

void IWRAM_CODE effect_kaleido_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
  exposure_palette((uint16_t*)0x5000000, huuuPal, 128 + abs(sin(frame << 4) >> 2));
  UpdateKaleidoscope(triangle_list, KALEIDO_SIDES, frame);

  int num_triangles = 0;
  for (uint16_t i = 0; i < KALEIDO_SIDES; ++i) {
    triangle_t t = triangle_list[i];

    t.vertices[0].position.x += 120;
    t.vertices[1].position.x += 120;
    t.vertices[2].position.x += 120;

    t.vertices[0].position.y += 80;
    t.vertices[1].position.y += 80;
    t.vertices[2].position.y += 80;

    clip_buffer[0] = t.vertices[0];
    clip_buffer[1] = t.vertices[1];
    clip_buffer[2] = t.vertices[2];

    uint8_t num_vertices = sutherland_hodgman_clipping((vertex_t *)&clip_buffer, 3);

    if (num_vertices >= 3) {
      uint16_t *texture = (uint16_t*)huuuBitmap;
      
      texture_polygon(clip_buffer, num_vertices, texture );
    }
  }

  uint16_t *mirror = target + 60;
  uint8_t b0, b1;

  /* now copy & mirror the left part ^_^ */
  for (uint16_t j = 0; j < 160; ++j) {
    for (uint16_t i = 0; i < 60; ++i) {
      unpack_color(b0, b1, target[60 - i]);
      mirror[i] = (b0 << 8) | b1;
    }
    target += 120;
    mirror += 120;
  }

  move_table_sprites((uint16_t*)0x601C000, tex_godwatbotBitmap, lut_ball_tiled, 0, 0, (frame << 8) + 3 * frame & 0x3fff);  
}