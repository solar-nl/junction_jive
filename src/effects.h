#ifndef __EFFECT_H
#define __EFFECT_H

#include <stdint.h>
#include "demo.h"

void move_table(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t texture_width, uint16_t t);
void move_table_sprites(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t t);
//void upscale_uv_table(uint16_t *target, uint8_t *texture, int *table, uint16_t t);

void bloom(uint16_t *target, const uint8_t bloom_bias, const uint8_t bloom_threshold);
void motion_blur(uint16_t *target);
void zoom_blur(uint16_t *target);

void fade_palette(uint16_t *dest, const uint16_t *palette, uint8_t red, uint8_t green, uint8_t blue, uint8_t v);
void exposure_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void invert_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void scale_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void add_palette(uint16_t *dest, const uint16_t *palette, int16_t v);
void cycle_palette();
void rotate_background(uint16_t angle, uint16_t zoom);

void effect_flowery_init();
void effect_flowery_destroy();
void effect_flowery_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_kaleido_init();
void effect_kaleido_destroy();
void effect_kaleido_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_torus_init();
void effect_torus_destroy();
void effect_torus_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_zwieber_init();
void effect_zwieber_destroy();
void effect_zwieber_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_greetings_init();
void effect_greetings_destroy();
void effect_greetings_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_credits_init();
void effect_credits_destroy();
void effect_credits_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_junction_init();
void effect_junction_destroy();
void effect_junction_update(uint16_t *target, uint32_t frame, uint16_t sync);

void effect_end_init();
void effect_end_destroy();
void effect_end_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_end = {
	effect_end_init,
	effect_end_update,
	effect_end_destroy,
};

static const struct scene_t effect_junction = {
	effect_junction_init,
	effect_junction_update,
	effect_junction_destroy,
};

static const struct scene_t effect_credits = {
	effect_credits_init,
	effect_credits_update,
	effect_credits_destroy,
};

static const struct scene_t effect_greetings = {
	effect_greetings_init,
	effect_greetings_update,
	effect_greetings_destroy,
};

static const struct scene_t effect_zwieber = {
	effect_zwieber_init,
	effect_zwieber_update,
	effect_zwieber_destroy,
};

static const struct scene_t effect_torus = {
	effect_torus_init,
	effect_torus_update,
	effect_torus_destroy,
};

static const struct scene_t effect_kaleido = {
	effect_kaleido_init,
	effect_kaleido_update,
	effect_kaleido_destroy,
};

static const struct scene_t effect_flowery = {
	effect_flowery_init,
	effect_flowery_update,
	effect_flowery_destroy,
};

extern int uvtable[16 * 8 * 3];
extern unsigned short generated_palette[256];


#endif /* __EFFECT_H */