#include <gba_video.h>
#include "fixedmath.h"
#include "vcount.h"

uint16_t glitch_count = 0;

void vcount_vignette()
{
  uint16_t scanline = min(REG_VCOUNT, 159);

  uint8_t ramp = 0;
  if (scanline < 32) {
    ramp = 16 - (scanline >> 1);
  }
  else if (scanline >= 160-32) {
    ramp = 16 - ((160 - scanline) >> 1);
  }

  REG_BLDCNT |= 0x00FF | ((0x0010) & 63);
  REG_BLDY = ramp;
}

void vcount_glitch()
{
  uint16_t scanline = min(REG_VCOUNT, 159);

  int16_t amp = sin(glitch_count >> 9) >> 3;
  int16_t phase = div((sin(((scanline << 5) + (glitch_count >> 6)))), (320 - amp * amp));

  REG_BG2PA = 256 - phase * phase;

  ++glitch_count;
}

void vcount_vhs()
{
  const int w = 10;
  int a = sin_lut[(glitch_count >> 9)&255] >> 4;
  int scanline_y = 50 + a * a;
  int phase = 0;
  int scanline = REG_VCOUNT;
  if (scanline > scanline_y - w && scanline < scanline_y + w)
  {
    phase = (scanline - scanline_y - w + 64) * 6;
    REG_BG2PA = 256-(sin_lut[(phase)&255] >> 2);
  }
  else
    REG_BG2PA = 256;

  ++glitch_count;
}
