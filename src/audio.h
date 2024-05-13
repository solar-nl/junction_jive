#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>

void play_audio(uint8_t *music, uint32_t size, uint8_t bpm);
void stop_audio();
void update_audio();
uint16_t get_audio_position();

#endif//__AUDIO_H