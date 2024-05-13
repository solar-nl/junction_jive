#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

void effect_greetings_init()
{
    current_model.position.z = -500;
    current_model.euler_angles.y = 8;

    set_palette(grammofoonPal);

    set_overlay(0, 0, 0, 0, 0);

    set_sprite_palette(tex_godwatbotPal);
    set_sprites_transparency(10, 16);    
}

void effect_greetings_destroy()
{

}

void effect_greetings_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    current_model.euler_angles.z = frame << 2;
    current_model.position.z = -550 + (cos(frame << 3) >> 1);

    int time = frame << 2;

    int x = (cos(time >> 2) >> 1);
    int y = (sin(time >> 1) >> 1);

   fast_copy(target, (uint16_t*)(greetingsBitmap + ((frame * 30) >> 3) * 120), 120 * 160);

    if (frame < 32) {
        int fade_to_black = min(frame << 5, 255);
        fade_palette((unsigned short*)0x5000000, grammofoonPal, 0, 0, 0, 255 - fade_to_black);
        fade_palette((unsigned short*)0x5000200, tex_godwatbotPal, 0, 0, 0, 255 - fade_to_black);
    }

    move_table_sprites((uint16_t*)0x601C000, tex_godwatbotBitmap, lut_torus_tiled, 0, 0, (frame << 8) + 3 * frame & 0x3fff);  
}