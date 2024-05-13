#include <gba_video.h>
#include <gba_sprites.h>
#include "memory.h"
#include "sprites.h"
#include "fixedmath.h"

#define GBA_BLEND(a, b) (a + (b << 8))

OBJATTR oam_entries[128]; 
OBJAFFINE *affine_entries = (OBJAFFINE*)oam_entries;

uint32_t sprite_address = 0;
uint32_t tile_address = 0;

void set_sprites_as_background()
{
	REG_BLDCNT = 0x0440;//1 << 10 | 0x40;
	REG_BLDALPHA = 0x1000;//GBA_BLEND(0, 16);
}

void set_framebuffer_as_mask() 
{
	REG_BLDCNT = 0x2050;
	REG_BLDALPHA = 0;
}

void set_sprites_transparency(int a, int b)
{
	REG_BLDCNT = 0x10 | 0x0400 | 0x40;
	REG_BLDALPHA = GBA_BLEND(max(a, 0), max(b, 0));
}

void commit_sprites()
{
  uint16_t *sprites = (uint16_t*)oam_entries;
  uint16_t *oam = (uint16_t*)OAM;
  fast_copy(oam, sprites, 512);	
}

void set_sprite_palette(const uint16_t *palette)
{
	fast_copy((uint16_t*)0x5000200, palette, 256);
}

void set_sprite(uint8_t id, int xpos, int ypos, uint8_t size, uint8_t rotate, uint8_t transparent, const uint16_t *image)
{
	int att0 = 0;
	int att1 = 0;
	
	if (size == 64)
		att1 |= ATTR1_SIZE_64;
	else if (size == 32)
		att1 |= ATTR1_SIZE_32;
	else if (size == 16)
		att1 |= ATTR1_SIZE_16;
	else if (size == 8)
		att1 |= ATTR1_SIZE_8;

	affine_entries[id].pa = 137;
	affine_entries[id].pd = 204;
	
	att0 |= ypos;
	att0 |= ATTR0_COLOR_256 | ATTR0_SQUARE | ATTR0_MOSAIC;
	
	att1 |= xpos;
	if (rotate) {
		att0 |= 0x0100;
		att0 |= 0x0100;
		att1 |= id << 9;
	}

	if (transparent) {
		att0 |= 0x0400;
	}
	
	oam_entries[id].attr0 = att0;
	oam_entries[id].attr1 = att1;
	oam_entries[id].attr2 = 512 + sprite_address;

	if (size == 64)
		sprite_address += 128;
	else if (size == 32)
		sprite_address += 32;
	else if (size == 16)
		sprite_address += 8;
	else if (size == 8)
		sprite_address += 2;
	
	uint16_t *dest = (uint16_t*)(0x601C000 + (uint16_t)tile_address);
	fast_copy(dest, image, (size * size) >> 1);
	
	tile_address += (size * size);
}

void clear_sprites()
{
	int i;
	for (i = 0; i < 128; i++) {
		oam_entries[i].attr0 = 0;
		oam_entries[i].attr1 = 0;
		oam_entries[i].attr2 = 0;
		oam_entries[i].dummy = 0;
	}
	sprite_address = 0;
	tile_address = 0;
}

void rotate_sprite(uint8_t id, uint8_t angle, int xscale, int yscale)
{
	affine_entries[id].pa = (xscale * cos(angle)) >> 8;
	affine_entries[id].pb = (yscale * sin(angle)) >> 8;
	affine_entries[id].pc = ((-xscale) * sin(angle)) >> 8;
	affine_entries[id].pd = (yscale * cos(angle)) >> 8;
}

void set_sprite_attributes(uint8_t id, uint16_t attrib0, uint16_t attrib1, uint16_t attrib2, uint16_t attrib3)
{
	oam_entries[id].attr0 = attrib0;
	oam_entries[id].attr1 = attrib1;
	oam_entries[id].attr2 = attrib2;
	oam_entries[id].dummy = attrib3;
}

void set_sprite_position(const uint8_t id, int xpos, int ypos)
{
	oam_entries[id].attr0 &= 0xFF00;
	oam_entries[id].attr0 |= ypos;
	oam_entries[id].attr1 &= 0xFE00;
	oam_entries[id].attr1 |= xpos;
}

void flip_sprite(const uint8_t id, const uint8_t flag)
{
	if (flag == FLIP_HORIZONTAL) {
		oam_entries[id].attr1 ^= 0x1000;
	}
	else if (flag == FLIP_VERTICAL) {
		oam_entries[id].attr1 ^= 0x2000;
	}
}

void set_overlay(const uint16_t *image1, const uint16_t *image2, const uint16_t *image3, const uint16_t *image4, const uint16_t *palette)
{
	set_sprites_transparency(0, 16);
	
	clear_sprites();

	set_sprite_palette(palette);
 
	fast_copy((uint16_t*)0x601C000, image1, 2048);
	fast_copy((uint16_t*)0x601D000, image2, 2048);
	fast_copy((uint16_t*)0x601E000, image3, 2048);
	fast_copy((uint16_t*)0x601F000, image4, 2048);

	set_sprite_attributes(0, 0x27E7, 0xC1FB, 0x0200, 0);
	set_sprite_attributes(1, 0x27E7, 0xC273, 0x0280, 0);
	set_sprite_attributes(2, 0x2738, 0xC5FB, 0x0300, 0);
	set_sprite_attributes(3, 0x2738, 0xC673, 0x0380, 0);

	rotate_sprite(0, 0, 136, 204);
	rotate_sprite(1, 0, 136, 204);
	rotate_sprite(2, 0, 136, 204);
	rotate_sprite(3, 0, 136, 204);

	commit_sprites();
}

void set_mosaic(const uint16_t bgx, const uint16_t bgy, const uint16_t objx, const uint16_t objy)
{
	REG_MOSAIC = ((bgx) + (bgy << 4) + (objx << 8) + (objy << 12));
}
