#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

extern const unsigned short tex_skydomePal[];
extern const unsigned short tex_skydomeBitmap[];
extern const unsigned short tex_skydome_128Bitmap[];

extern const int16_t boll_solid_env_decimated_model[] ;

void effect_zwieber_init()
{
    load_model(potig_model, 112, MODEL_ENVMAPPED | MODEL_SOFT | MODEL_NOCLIP, 1, 0);
    finalize_mesh(112);

    current_model.position.z = -850;

    clear_sprites();
    commit_sprites();
}

void effect_zwieber_destroy()
{
    current_model.euler_angles.x = current_model.euler_angles.y = current_model.euler_angles.z = 0;
    current_model.position.x = current_model.position.y = current_model.position.z = 0;
}


void IWRAM_CODE effect_zwieber_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    current_model.euler_angles.x = 90 + sin((frame * 8) >> 2) >> 1;
    current_model.euler_angles.y = 90 + -cos((frame * 3) >> 2) >> 1;
    current_model.euler_angles.z = 64; 

    int time = frame << 2;
    int t = ((-8 & 0xFF) << 8) | ((frame) & 0xFF) & 0xFFFF;
    move_table(target, tex_skydomeBitmap, lut_skydome, 60, 0, 256, t);  

    render(&current_model);

    exposure_palette((uint16_t*)0x5000000, tex_skydomePal, 128 + abs(sin(frame >> 0) >> 2));
}