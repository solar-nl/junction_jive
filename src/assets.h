#ifndef __ASSETS_H
#define __ASSETS_H

/* music */

extern const uint8_t junction_jive_data[];

/* Junction jive */

extern const unsigned short junction_jive_title_1Bitmap[];
extern const unsigned short junction_jive_title_1Pal[];

/* Solar logo */
extern const unsigned short solar_logo_1Tiles[];
extern const unsigned short solar_logo_2Tiles[];
extern const unsigned short solar_logo_3Tiles[];
extern const unsigned short solar_logo_4Tiles[];

extern const unsigned short solar_logo_fbPal[];
extern const unsigned short solar_logo_fbBitmap[];

/* Greetings bitmap */
extern const unsigned short greetingsBitmap[153600];

/* 3d models */
extern const short potig_model[];
extern const short ster_model[]; // 126

extern const unsigned short tex_bushaltePal[];
extern const unsigned short tex_bushalteBitmap[];

extern const int bushalte_model[];

extern const int16_t grammofoon_model[];
extern const unsigned short grammofoonPal[];
extern const unsigned short grammofoonBitmap[];

/* STS textures */
extern const unsigned short ja_botPal[];
extern const unsigned short ja_botBitmap[]; 
extern const unsigned short huuuPal[];
extern const unsigned short huuuBitmap[];
extern const unsigned short crashPal[];
extern const unsigned short crashBitmap[];
extern const unsigned short botBitmap[];
extern const unsigned short botPal[];
extern const unsigned short wel_fijnPal[];
extern const unsigned short wel_fijnBitmap[];
extern const unsigned short coat3Pal[];
extern const unsigned short coat3Bitmap[];

extern const unsigned short tex_coat3_128Pal[];
extern const unsigned short tex_coat3_128Bitmap[];

extern const unsigned short envmap_zwierBitmap[32768];
extern const unsigned short envmap_zwierPal[32768];
extern const unsigned short achtergrond_zwierBitmap[19200];

/* Solar text */
extern const unsigned short ravity_test1Pal[];
extern const unsigned short ravity_test_perlin1Tiles[];
extern const unsigned short ravity_test_perlin2Tiles[];
extern const unsigned short ravity_test_perlin3Tiles[];
extern const unsigned short ravity_test_perlin4Tiles[];

extern const unsigned short coat_test_perslin1Tiles[];
extern const unsigned short coat_test_perslin2Tiles[];
extern const unsigned short coat_test_perslin3Tiles[];
extern const unsigned short coat_test_perslin4Tiles[];

/* Sprites */
extern const unsigned short spr_blobPal[];
extern const unsigned short spr_blobTiles[];

/* model test */
extern const unsigned short envmapBitmap[];
extern const unsigned short envmapBitmap2[];
extern const unsigned short envmapPal[];

extern const unsigned short scene_oBitmap[32768];
extern const unsigned short scene_oPal[255];


extern const unsigned short tex_skydomePal[];
extern const unsigned short tex_skydomeBitmap[];

/* palettes */
extern const unsigned short master_gradient[256];
extern const unsigned short phong_gradient[256];
extern const unsigned short golden_gradient[256];
extern const unsigned short smooth_gradient[256];
extern const unsigned short solar_caPal[256];
extern const unsigned short ai_griet[256];

/* movetable data */
extern const unsigned short tunnellut[];
extern const unsigned short flower_lut_tiled[];
extern const unsigned short lut_torus_fb[];
extern const unsigned short lut_skydome[];

extern const unsigned short lut_torus_tiled[];
extern const unsigned short lut_skydome_tiled[];
extern const unsigned short lut_ball_tiled[];
extern const unsigned short lut_swirl_tiled[];
extern const unsigned short lut_tunnel_tiled[]; 

/* textures */
extern const unsigned short tex_godwatbotBitmap[8192];
extern const unsigned short tex_godwatbotPal[256];
extern const unsigned short metalBitmap[8192];
extern const unsigned short metalPal[256];

#endif /* __ASSETS_H */