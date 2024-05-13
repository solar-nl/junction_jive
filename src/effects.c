#include <gba_video.h>
#include <stdint.h>
#include "effects.h"
#include "memory.h"
#include "rendering.h"
#include "fixedmath.h"

#pragma GCC diagnostic ignored "-Wsequence-point"

extern uint16_t *backbuffer;

#define UV_TABLE_ROW 24 //45

int uvtable[16 * 8 * 3];
int shadingtable[20][15];
uint8_t bytebuffer[8];

IWRAM_CODE void move_table(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t texture_width, uint16_t t)
{
	uint32_t *target32 = (uint32_t*)(target);
	uint16_t *uv = (yoffset * 320) + xoffset + (table - 1);
  uint16_t mask;

  if (texture_width == 256) {
    mask = 0xFFFF;
  }
  else {
    mask = 0x3FFF;
  }

	int i = 0, j = 0;

	#define PIXEL8 {\
		*target32++ = (texture[((*(++uv))+t)&mask]) | (texture[((*(++uv))+t)&mask]<<8) | (texture[((*(++uv))+t)&mask]<<16 | (texture[((*(++uv))+t)&mask]<<24));\
		*target32++ = (texture[((*(++uv))+t)&mask]) | (texture[((*(++uv))+t)&mask]<<8) | (texture[((*(++uv))+t)&mask]<<16 | (texture[((*(++uv))+t)&mask]<<24));\
		*target32++ = (texture[((*(++uv))+t)&mask]) | (texture[((*(++uv))+t)&mask]<<8) | (texture[((*(++uv))+t)&mask]<<16 | (texture[((*(++uv))+t)&mask]<<24));\
		*target32++ = (texture[((*(++uv))+t)&mask]) | (texture[((*(++uv))+t)&mask]<<8) | (texture[((*(++uv))+t)&mask]<<16 | (texture[((*(++uv))+t)&mask]<<24));\
	}

	const uint16_t endline = 160;
	const uint16_t line = 320 - 240;

  do {
		i = 0;

    do {
			PIXEL8
			PIXEL8
			PIXEL8
		} while (++i < 5);

		uv += line;
    } while (++j < endline);
}

IWRAM_CODE void move_table_sprites(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t t)
{
  uint32_t *target32 = (uint32_t*)(target);
  uint16_t *uv = table - 1;

  int i = 0, j = 0;

  #define PIXEL4 {\
    *target32++ = (texture[((*(++uv))+t)&0x3FFF]) | (texture[((*(++uv))+t)&0x3FFF]<<8) | (texture[((*(++uv))+t)&0x3FFF]<<16 | (texture[((*(++uv))+t)&0x3FFF]<<24));\
    *target32++ = (texture[((*(++uv))+t)&0x3FFF]) | (texture[((*(++uv))+t)&0x3FFF]<<8) | (texture[((*(++uv))+t)&0x3FFF]<<16 | (texture[((*(++uv))+t)&0x3FFF]<<24));\
  }

  const uint16_t endline = 128;
  const uint16_t line = 0;

  do {
    i = 0;

    do {
      PIXEL4
      PIXEL4
    } while (++i < 8);

    uv += line;
  } while (++j < endline);
}

/*
IWRAM_CODE void upscale_uv_table(uint16_t *target, uint8_t *texture, int *table, uint16_t t)
{
	int u1, v1, u2, v2, u3, v3, u4, v4, u ,v;
	int s1, t1, s2, t2;

	int y = 0;
	int ymul = 0;
	int du1, du2, dv1, dv2, eu1, eu2, d1, d2, d3, d4, zdu1, zdu2;
	int tempu1, tempu2, tempv1, tempv2, tempz1, tempz2 ,z1, z2, z, vz1;

	int *uvs = table;

	int i, j;
	int color;
	int offset;

   for (j = 0; j < (15>>1); j++)
    {
        for (i = 0; i < (7>>1); i++) 
        {   
            u1 = *uvs++;
            v1 = *uvs++;
            d1 = *uvs++;

            u2 = *uvs++;
            v2 = *uvs++;
            d2 = *uvs++;

            uvs += UV_TABLE_ROW - 6;

            u3 = *uvs++;
            v3 = *uvs++;
            d3 = *uvs++;

            u4 = *uvs++;
            v4 = *uvs++;
            d4 = *uvs++;

            uvs -= UV_TABLE_ROW;
            uvs -= 3;

            du1 = (((u2 - u1) << 16) >> 4);
            du2 = (((u4 - u3) << 16) >> 4);
            dv1 = (((v2 - v1) << 16) >> 4);
            dv2 = (((v4 - v3) << 16) >> 4);
            zdu1 = (((d2 - d1) << 16) >> 4);
            zdu2 = (((d4 - d3) << 16) >> 4);

            ymul = (((j << 4)) * 120) + (i << 4) - 4 + 120 * 4;
            for (y = 0; y < 16; y += 2)
            {
                tempu1 = tempu2 = tempv1 = tempv2 = tempz1 = tempz2 = 0;
                {      

                    #define INTERPOLATE_SPR(plus) \
                    { \
                      tempu1 += du1; tempu2 += du2; s1 = u1 + (tempu1 >> 16); s2 = u3 + (tempu2 >> 16); \
                      tempv1 += dv1; tempv2 += dv2; t1 = v1 + (tempv1 >> 16); t2 = v3 + (tempv2 >> 16); \
                      tempz1 += zdu1; tempz2 += zdu2; z1 = d1 + (tempz1 >> 16); z2 = d3 + (tempz2 >> 16);\
                      vz1 = (((z2 - z1) << 16) >> 4); \
                      eu1 = (((s2 - s1) << 16) >> 4); eu2 = (((t2 - t1) << 16) >> 4); \
                      u = (s1 + ((eu1 * y) >> 16)); v = (t1 + ((eu2 * y) >> 16)); z = (z1 + ((vz1 * y) >> 16)) >> 5; \
                      offset = ymul + add + 8; \
                      color = texture[((v << 7) + u) & 0x3FFF] - 224 + (z << 5); \
											color = (color << 8) | color;\
											target[offset] = target[offset + 120] = color; \
                    }

                    INTERPOLATE_SPR(0)
                    INTERPOLATE_SPR(1)
                    INTERPOLATE_SPR(2)
                    INTERPOLATE_SPR(3)
                    INTERPOLATE_SPR(4)
                    INTERPOLATE_SPR(5)
                    INTERPOLATE_SPR(6)
                    INTERPOLATE_SPR(7)
                    INTERPOLATE_SPR(8)
                    INTERPOLATE_SPR(9)
                    INTERPOLATE_SPR(10)
                    INTERPOLATE_SPR(11)
                    INTERPOLATE_SPR(12)
                    INTERPOLATE_SPR(13)
                    INTERPOLATE_SPR(14)
                    INTERPOLATE_SPR(15)
                }
                ymul += 128;
            } 
        }
        uvs += 3;
    }   
}
*/

void IWRAM_CODE store(uint16_t *target, uint16_t offset, uint16_t value)
{
  target[offset] |= value;
}

void IWRAM_CODE bloom(uint16_t *target, const uint8_t bloom_bias, const uint8_t bloom_threshold)
{
  int i, j, u, x, y;
  int offset = 0;

  for (i = 0; i < 15; i++)
  {
    shadingtable[0][i] = 0;
    shadingtable[19][i] = 0;
  }

  for (j = 0; j < 20; j++)
  {
    shadingtable[j][0] = 0;
    shadingtable[j][14] = 0;
  }

  uint8_t *texels = (uint8_t*)target;
  
	for (y = 1; y < 19; y++)
	for (x = 1; x < 14; x++)  
	{
		int paletteIndex = texels[(y << 4) * 120 + (x << 4)];

		int color = ((unsigned short*)0x5000000)[paletteIndex];

		int r = (color >> 10) & 31;
		int g = (color >> 5) & 31;
		int b = color & 31;

		color = (r + g + b) + bloom_bias;

		shadingtable[y][x] = color > bloom_threshold ? 200 : 0;
	}

	for (y = 1; y < 19; y++)
	for (x = 1; x < 14; x++)  
	{
		shadingtable[y][x] = div((shadingtable[y][x] + shadingtable[y-1][x] + shadingtable[y+1][x] + shadingtable[y][x-1] + shadingtable[y][x+1]), 8);
	}

	int color = 0;
	int u1, u2, u3, u4;
	int s1,s2;
	const int tab = 15;
	offset = 0, x = 0, y = 0;
	int tablepos = 0;
	int ymul = 0;
	int du1, du2, eu1;
	int tempu1, tempu2;

	for (j = 0; j < 20-1; j++)
	{
		for (i = 0; i < 15-1; i++)
		{  
			u1 = shadingtable[j][i];
			u2 = shadingtable[j][i+1];
			u3 = shadingtable[j+1][i];
			u4 = shadingtable[j+1][i+1];
			
			du1 = (((u2 - u1) << 8) >> 3);
			du2 = (((u4 - u3) << 8) >> 3);

			ymul = (j << 3) * 120 + (i << 3);

			for (y = 0; y < 8; y += 2)
			{
				s1 = u1;
				s2 = u3;
				tempu1 = tempu2 = 0;
				{                           
					#define ADD(add) \
					{ \
						tempu1 += du1; tempu2 += du2; s1 = u1 + (tempu1 >> 8); s2 = u3 + (tempu2 >> 8); \
						eu1 = (((s2 - s1) << 8) >> 3); \
						u = (s1 + ((eu1 * y) >> 8)); \
						offset = ymul + add; \
						color = ((u << 8) | u); target[offset] |= color; target[offset+120] |= color; \
					}

					ADD(0)
					ADD(1)
					ADD(2)
					ADD(3)
					ADD(4)
					ADD(5)
					ADD(6)
					ADD(7)
				}
				ymul += 240;
			} 
		}
		tablepos += tab;
	}	   
}

void IWRAM_CODE motion_blur(uint16_t *target)
{
	int c;
	int b0, b1;

  for (int i = 0; i < (VIEWPORT_WIDTH * VIEWPORT_HEIGHT >> 1); i++) {
    c = *backbuffer++;
    b0 = (c & 0xFF);
    b1 = ((c >> 8) & 0xFF);

		b0 = (b0 >> 1) + (b0 >> 2);
		b1 = (b1 >> 1) + (b1 >> 2);

    *target++ = (b1 << 8) | b0; 
  }
}

void IWRAM_CODE zoom_blur(uint16_t *target)
{
	const int step = 192;
	const int start = 20 * 120 + 15;

	int offset, left, right, up, down;
	int curx = start;
	int u, v = 0;

	for (int j = 0; j < 160; ++j, u = 0) {
		for (int i = 0; i < 30; ++i) {
			#define BLUR_STEP { \
				offset = curx + (u >> 8); \
				u += step; \
			}
			#define BLUR_PIXEL { \
				left = (backbuffer[offset - 1]); \
				right = (backbuffer[offset + 1]);\
				up = (backbuffer[offset - 120]);\
				down = (backbuffer[offset + 120]); \
				unpack_color(bytebuffer[0], bytebuffer[4], left);\
				unpack_color(bytebuffer[1], bytebuffer[5], right);\
				unpack_color(bytebuffer[2], bytebuffer[6], up);\
				unpack_color(bytebuffer[3], bytebuffer[7], down);\
				bytebuffer[0] = (bytebuffer[0] + bytebuffer[1] + bytebuffer[2] + bytebuffer[3]) >> 2;\
				bytebuffer[1] = (bytebuffer[4] + bytebuffer[5] + bytebuffer[6] + bytebuffer[7]) >> 2;\
				bytebuffer[0] = (bytebuffer[0] >> 1) + (bytebuffer[0] >> 2);\
				bytebuffer[1] = (bytebuffer[1] >> 1) + (bytebuffer[1] >> 2);\
				*target++ = (bytebuffer[1] << 8) | bytebuffer[0];\
			}

			BLUR_STEP; BLUR_PIXEL;
			BLUR_STEP; BLUR_PIXEL;
			BLUR_STEP; BLUR_PIXEL;
			BLUR_STEP; BLUR_PIXEL;
		}
		curx = start + 120 * ((v += step) >> 8);
	}
}

void IWRAM_CODE fade_palette(uint16_t *dest, const uint16_t *palette, uint8_t red, uint8_t green, uint8_t blue, uint8_t v)
{
  uint16_t r, g, b, color;

  for (uint16_t i = 0; i < 256; ++i)
  {
    color = *palette++;

    r = (color >> 10) & 31;
    g = (color >> 5) & 31;
    b = color & 31;

    r = lerp(r, red, v);
    g = lerp(g, green, v);
    b = lerp(b, blue, v);

    *dest++ = (r << 10) | (g << 5) | b;    
  }  
}

void IWRAM_CODE exposure_palette(uint16_t *dest, const uint16_t *palette, uint8_t v)
{
    int r, g, b, color;

    for (uint16_t i = 0; i < 256; ++i)
    {
        color = *palette++;

        r = (color >> 10) & 31;
        g = (color >> 5) & 31;
        b = color & 31;
        
        r = min(31, (r * (256 + v)) >> 8);
        g = min(31, (g * (256 + v)) >> 8);
        b = min(31, (b * (256 + v)) >> 8);

        *dest++ = (r << 10) | (g << 5) | b;
    }
}

void invert_palette(uint16_t *dest, const uint16_t *palette, uint8_t v)
{
  uint16_t r, g, b, color;

  for (uint16_t i = 0; i < 255; ++i)
  {
    color = *palette++;

    r = (color >> 10) & 31;
    g = (color >> 5) & 31;
    b = color & 31; 

    r += (((31 - r) - r) * v) >> 8;
    g += (((31 - g) - g) * v) >> 8;
    b += (((31 - b) - b) * v) >> 8;

    *dest++ = (r << 10) | (g << 5) | b;
  }
}

void scale_palette(uint16_t *dest, const uint16_t *palette, uint8_t v)
{
  uint16_t r, g, b, color;
  uint16_t scale = v + 256;

  for (uint16_t i = 0; i < 255; ++i)
  {
    color = *palette++;

    r = (color >> 10) & 31;
    g = (color >> 5) & 31;
    b = color & 31; 

    r = (r * (scale)) >> 8;
    g = (g * (scale)) >> 8;
    b = (b * (scale)) >> 8;

    *dest++ = (r << 10) | (g << 5) | b;
  }
}

void add_palette(uint16_t *dest, const uint16_t *palette, int16_t v)
{
  uint16_t r, g, b, color;

  for (uint16_t i = 0; i < 255; ++i)
  {
    color = *palette++;

    r = (color >> 10) & 31;
    g = (color >> 5) & 31;
    b = color & 31; 

    r += v;
    g += v;
    b += v;

    r = max(r, 1);
    g = max(g, 1);
    b = max(b, 1);

    r = min(r, 31);
    g = min(g, 31);
    b = min(b, 31);
    
    *dest++ = (r << 10) | (g << 5) | b;
  }
}

void crossfade_image(uint16_t *target, uint8_t* bitmap1, uint8_t* bitmap2, uint8_t v) {
    uint16_t x, y, c0, c1;
    uint16_t inv = 256 - v;

    for (y = 0; y < VIEWPORT_HEIGHT; y++) {
        for (x = 0; x < VIEWPORT_WIDTH; x += 2) {
            c0 = ((*bitmap1++ * inv + *bitmap2++ * v) >> 8);
            c1 = ((*bitmap1++ * inv + *bitmap2++ * v) >> 8);

            *target++ = (c1 << 8) | c0;
        }
    }
}

void rotate_background(uint16_t angle, uint16_t zoom)
{
  int16_t pa, pb, pc, pd;

  REG_BG2PA = pa = (cos(angle) * zoom) >> 8; 
  REG_BG2PB = pb = -(sin(angle) * zoom) >> 8;
  REG_BG2PC = pc = (sin(angle) * zoom) >> 8;
  REG_BG2PD = pd = (cos(angle) * zoom) >> 8;

  REG_BG2X = ((120 << 8) - (120 * pa + 80 * pb));
  REG_BG2Y = ((80 << 8) - (120 * pc + 80 * pd));
}

void IWRAM_CODE cycle_palette() 
{
    uint16_t first_color;
    first_color = generated_palette[255];
    for (int i = 255; i > 0; --i) {
        generated_palette[i] = generated_palette[i - 1];
    }

    generated_palette[0] = first_color;
}