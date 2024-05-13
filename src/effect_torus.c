#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

void effect_torus_init()
{
    set_sprites_transparency(10, 16);
}

void effect_torus_destroy()
{

}

void effect_torus_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    if (frame < 8) {
        int fader = min(frame << 5, 255);
        fader = fader > 250 ? 255 : fader;
        fade_palette((unsigned short*)0x5000000, huuuPal, 31, 31, 31, 255 - fader);
        fade_palette((unsigned short*)0x5000200, tex_godwatbotPal, 31, 31, 31, 255 - fader);
    }

    int time = frame << 2;

    int x = (cos(time >> 2) * 140) >> 8;
    int y = (sin(time >> 1) * 140) >> 8;

    move_table(target, huuuBitmap, lut_torus_fb, 38+(x>>2), 80+(y>>2)-20, 256, (3 * -frame << 8) & 0xffff);
    move_table_sprites((uint16_t*)0x601C000, tex_godwatbotBitmap, lut_ball_tiled, 0, 0, (frame << 8) + 3 * frame & 0x3fff);
}
