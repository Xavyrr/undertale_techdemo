#pragma once
// ifndef for compilers that don't support #pragma once.
#ifndef UT_TD_SOUND
#define UT_TD_SOUND

#include <3ds.h>
#include <stdbool.h>
#include <tremor/ivorbisfile.h>

struct sound {
        OggVorbis_File vf;
        ndspWaveBuf waveBuf;
        float mix[12];
        unsigned long pos;
        long status;
        int section;
        int channel;
        char *buf;
};

void audio_init(void);
struct sound* sound_create();
void audio_load_ogg(const char *audio, struct sound *sound);
void sound_loop(struct sound *sound);
void sound_stop(struct sound *sound);
void audio_stop(void);

#endif
