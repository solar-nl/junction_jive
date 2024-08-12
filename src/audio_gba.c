#include <gba_dma.h>
#include <gba_interrupt.h>
#include <gba_sound.h>
#include <gba_timers.h>
#include "audio.h"
#include "fixedmath.h"

// Audio configuration constants
const uint16_t sample_rate = 16000;  // 16 kHz sample rate
const uint16_t ticks_per_sample = (1 << 24) / sample_rate;  // Timer ticks per audio sample
const uint16_t cycles_per_vblank = (1 << 24) / 60;  // Timer cycles per vertical blank (60 Hz)

// Global variables for music playback
uint8_t *music_data = 0;  // Pointer to the music data
uint16_t music_bpm = 0;   // Beats per minute of the music
uint32_t music_tick = 0;  // Current playback position (in ticks)
uint32_t music_length = 0;  // Total length of the music (in ticks)
uint8_t music_is_playing = 0;  // Flag to indicate if music is currently playing

/**
 * @brief Starts playing audio
 * 
 * This function initializes the audio system and starts playback.
 * 
 * @param music Pointer to the audio data
 * @param size Size of the audio data in 4-byte words
 * @param bpm Beats per minute of the music
 */
void play_audio(uint8_t *music, uint32_t size, uint8_t bpm)
{
    music_data = music;
    // Calculate total length in ticks, accounting for sample rate and vblank frequency
    music_length = size * ticks_per_sample * (1.0 / cycles_per_vblank);
    music_tick = 0;
    music_bpm = bpm;

    // Enable sound hardware
    REG_SOUNDCNT_X = SNDSTAT_ENABLE;
    // Configure stereo sound, 100% volume, reset FIFO
    REG_SOUNDCNT_H = SNDA_VOL_100 | SNDA_RESET_FIFO | SNDA_L_ENABLE | SNDA_R_ENABLE;

    // Set up DMA channel 1 for audio playback
    REG_DMA1SAD = (uintptr_t)music_data;  // Source address
    REG_DMA1DAD = (uintptr_t)&REG_FIFO_A;  // Destination address (audio FIFO)
    REG_DMA1CNT = DMA_ENABLE | DMA32 | DMA_DST_FIXED | DMA_REPEAT | DMA_SPECIAL;

    // Configure Timer 0 for sample rate timing
    REG_TM0CNT = 0;  // Stop the timer
    REG_TM0CNT_L = 65556 - ticks_per_sample;  // Set the timer period
    REG_TM0CNT_H = TIMER_START;  // Start the timer

    music_is_playing = 1;
}

/**
 * @brief Stops audio playback
 */
void stop_audio()
{
    music_is_playing = 0;
}

/**
 * @brief Updates the audio system
 * 
 * This function should be called every frame (typically in vblank).
 * It handles looping and stopping of the audio.
 */
void update_audio()
{   
    if (!music_is_playing) {
        // Stop DMA transfer if music is not playing
        REG_DMA1CNT = 0;
        REG_DMA1SAD = 0;
        REG_DMA1CNT = 0;
    }

    if (music_tick >= music_length) {
        // Reset playback to the beginning when the end is reached
        REG_DMA1CNT = 0;
        REG_DMA1SAD = (uintptr_t)music_data;
        REG_DMA1CNT = DMA_ENABLE | DMA32 | DMA_DST_FIXED | DMA_REPEAT | DMA_SPECIAL;

        music_tick = 0;
    } 
    else {
        ++music_tick;
    }
}

/**
 * @brief Get the current position in the audio playback
 * 
 * @return uint16_t Current position in beats
 */
uint16_t get_audio_position()
{
    // Convert ticks to beats based on BPM
    // The magic number 3575.4 is likely derived from (60 * sample_rate) / (ticks_per_sample * 256)
    return music_tick * music_bpm / 3575.4;
}