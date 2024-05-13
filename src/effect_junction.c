#include "effects.h"
#include "rendering.h"
#include "memory.h"
#include "sprites.h"
#include "fixedmath.h"
#include "assets.h"

void effect_junction_init()
{
    set_overlay(0, 0, 0, 0, 0);
    set_sprites_transparency(16, 16);
}

void effect_junction_destroy()
{
}

void effect_junction_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    static int fade_to_white = 0;
    
    if (frame < 32) {
        int fade_to_black = min(frame << 5, 255);

        fade_palette((unsigned short*)0x5000000, junction_jive_title_1Pal, 0, 0, 0, 255 - fade_to_black);
        fade_palette((unsigned short*)0x5000200, tex_godwatbotPal, 0, 0, 0, 255 - fade_to_black);
    }

    if (frame >= 260) {
        fade_to_white += 8;

        fade_palette((unsigned short*)0x5000000, junction_jive_title_1Pal, 31, 31, 31, min(fade_to_white, 255));
        fade_palette((unsigned short*)0x5000200, tex_godwatbotPal, 31, 31, 31, min(fade_to_white, 255));
    }

    fast_copy(target, junction_jive_title_1Bitmap, VIEWPORT_HEIGHT * VIEWPORT_WIDTH >> 1);

    move_table_sprites((uint16_t*)0x601C000, tex_godwatbotBitmap, lut_ball_tiled, 0, 0, ((frame << 8) + 3 * frame) & 0x3fff);
}