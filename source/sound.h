#pragma once
// ifndef for compilers that don't support #pragma once .
#ifndef UT_TD_SOUND
#define UT_TD_SOUND

#include <3ds.h>
#include <stdbool.h>

extern u8* buffer; // Buffering audio file
extern unsigned long buf_samples; // Audio file size
extern unsigned long buf_pos;
extern long mus_failure;
extern bool eof;

void audio_init(void);
void audio_load_ogg(const char *audio);
void audio_loop(long size);
void audio_stop(void);

#endif
