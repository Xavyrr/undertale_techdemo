#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include "sound.h"
#include <3ds.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int last_id = 0;

void audio_init() {
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
}

struct sound* sound_create() {
    struct sound *new_sound = malloc(sizeof(struct sound));
    if (new_sound == NULL) return NULL;

    new_sound->pos = 0;
    new_sound->channel = last_id++; // TODO: Better channel management.

    memset(&(new_sound->waveBuf), 0, sizeof(ndspWaveBuf));
    new_sound->waveBuf.looping = true;

    memset(new_sound->mix, 0, sizeof(new_sound->mix));
    new_sound->mix[0] = new_sound->mix[1] = 1.0;

    ndspChnSetInterp(new_sound->channel, NDSP_INTERP_LINEAR);
    ndspChnSetRate(new_sound->channel, 44100);
    ndspChnSetFormat(new_sound->channel, NDSP_FORMAT_STEREO_PCM16);
    ndspChnSetMix(new_sound->channel, new_sound->mix);

    return new_sound;
}

// Audio load/play
void audio_load_ogg(const char *name, struct sound *sound) {
	const long sample_size = 4;

    /// Copied from ivorbisfile_example.c
    FILE *mus = fopen(name, "rb");
    if (ov_open(mus, &sound->vf, NULL, 0)) {
        return;
    }
    sound->waveBuf.nsamples = (unsigned long)ov_pcm_total(&sound->vf,-1);

    sound->buf = linearAlloc(sound->waveBuf.nsamples * sample_size);
    sound->waveBuf.data_vaddr = sound->buf;

    int i;

    for (i = 0; i < 6; ++i) {
        sound_loop(sound);
    }

    ndspChnWaveBufAdd(sound->channel, &sound->waveBuf);
}

void sound_loop(struct sound *sound) {
    // if (mus_failure <= 0) return;

    long size = sound->waveBuf.nsamples * 4 - sound->pos;
    size = (size < 4096)? size : 4096; // min(size, 4096);

    sound->status = ov_read(&sound->vf, sound->buf + sound->pos, size, &sound->section);
    if (sound->status <= 0) {
        ov_clear(&sound->vf);
        if (sound->status < 0) ndspChnReset(sound->channel);
    }

    else sound->pos += sound->status;
}

void sound_stop(struct sound *sound) {
    ndspChnReset(sound->channel);
    GSPGPU_FlushDataCache(sound->buf, sound->waveBuf.nsamples * 4);
    linearFree(sound->buf);
    // memset (buffer, 0, size);
}

void audio_stop(void) {
    ndspExit();
}
