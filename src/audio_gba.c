#include <gba_dma.h>
#include <gba_interrupt.h>
#include <gba_sound.h>
#include <gba_timers.h>
#include "audio.h"
#include "fixedmath.h"

const uint16_t sample_rate = 16000;
const uint16_t ticks_per_sample = (1 << 24) / sample_rate;
const uint16_t cycles_per_vblank = (1 << 24) / 60;

uint8_t *music_data = 0;
uint16_t music_bpm = 0;
uint32_t music_tick = 0;
uint32_t music_length = 0;
uint8_t music_is_playing = 0;

void play_audio(uint8_t *music, uint32_t size, uint8_t bpm)
{
    music_data = music;
    music_length = size * ticks_per_sample * (1.0 / cycles_per_vblank);
    music_tick = 0;
    music_bpm = bpm;

    REG_SOUNDCNT_X = SNDSTAT_ENABLE;
    REG_SOUNDCNT_H = SNDA_VOL_100 | SNDA_RESET_FIFO | SNDA_L_ENABLE | SNDA_R_ENABLE;

    REG_DMA1SAD = (uintptr_t)music_data;
    REG_DMA1DAD = (uintptr_t)&REG_FIFO_A;
    REG_DMA1CNT = DMA_ENABLE | DMA32 | DMA_DST_FIXED | DMA_REPEAT | DMA_SPECIAL;

    REG_TM0CNT = 0; 
    REG_TM0CNT_L = 65556 - ticks_per_sample;
    REG_TM0CNT_H = TIMER_START;
    
    music_is_playing = 1;
}

void stop_audio()
{
    music_is_playing = 0;
}

void update_audio()
{   if (!music_is_playing) {
        REG_DMA1CNT = 0;
        REG_DMA1SAD = 0;
        REG_DMA1CNT = 0;
    }

    if (music_tick >= music_length) {
        REG_DMA1CNT = 0;
        REG_DMA1SAD = (uintptr_t)music_data;
        REG_DMA1CNT = DMA_ENABLE | DMA32 | DMA_DST_FIXED | DMA_REPEAT | DMA_SPECIAL;

        music_tick = 0;
    } 
    else {
        ++music_tick;
    }
}

uint16_t get_audio_position()
{
    return music_tick * music_bpm / 3575.4;
}