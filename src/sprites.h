#ifndef __SPRITES_H
#define __SPRITES_H

#include <stdint.h>

#define FLIP_HORIZONTAL 0
#define FLIP_VERTICAL   1

void commit_sprites();
void clear_sprites();
void set_sprites_transparency(int a, int b);
void set_framebuffer_as_mask();
void set_sprites_as_background();
void set_sprite_palette(const uint16_t *palette);
void set_sprite(uint8_t id, int xpos, int ypos, uint8_t size, uint8_t rotate, uint8_t transparent, const uint16_t *image);
void rotate_sprite(uint8_t id, uint8_t angle, int xscale, int yscale);
void set_overlay(const uint16_t *image1, const uint16_t *image2, const uint16_t *image3, const uint16_t *image4, const uint16_t *palette);
void set_sprite_position(const uint8_t id, int xpos, int ypos);
void flip_sprite(const uint8_t id, const uint8_t flag);
void set_mosaic(const uint16_t bgx, const uint16_t bgy, const uint16_t objx, const uint16_t objy);

#endif /* __SPRITES_H */
