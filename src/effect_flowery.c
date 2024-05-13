#include "effects.h"
#include "rendering.h"
#include "memory.h"
#include "sprites.h"
#include "fixedmath.h"
#include "assets.h"

void effect_flowery_init()
{
    set_overlay(0, 0, 0, 0, 0);
    set_palette(envmapPal);
    set_sprite_palette(envmapPal);

    set_sprites_transparency(10, 16);
}

void effect_flowery_destroy()
{
    set_sprites_transparency(16, 16);
}

void effect_flowery_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    int time = (frame << 2) - 64;

    int x = 0;
    int y = (sin(time >> 1) >> 1);
    int t = ((((time + x) & 0xFF) << 8) | ((time + y) & 0xFF)) & 0x7fff;
    
    move_table(target, envmapBitmap, tunnellut, 40+(x>>2), 80+(y>>2)-20, 128, t);
    move_table_sprites((uint16_t*)0x601C000, envmapBitmap, flower_lut_tiled, 0, 0, frame << 3);
}