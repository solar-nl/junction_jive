#include "audio.h"
#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

void effect_end_init()
{
    clear_screen(0x00);

    set_sprite_palette(ravity_test1Pal);

    fast_set((unsigned short*)0x5000000, 0, 128);

    set_sprite(0, 120-32-32, 80-32-32, 64, 0, 0, solar_logo_1Tiles);
    set_sprite(1, 120+32-32, 80-32-32, 64, 0, 0, solar_logo_2Tiles);
    set_sprite(2, 120-32-32, 80+32-32, 64, 0, 0, solar_logo_3Tiles);
    set_sprite(3, 120+32-32, 80+32-32, 64, 0, 0, solar_logo_4Tiles);

    commit_sprites();

    stop_audio();
}

void effect_end_destroy()
{
}

void effect_end_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    clear_screen(0x00);

    if (frame > 8) {
        while (1) {}
    }
}