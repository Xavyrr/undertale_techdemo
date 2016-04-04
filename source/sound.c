#include "tremor/ivorbiscodec.h"
#include "tremor/ivorbisfile.h"
#include "sound.h"
#include <3ds.h>

#include <string.h>
#include <stdbool.h>

u8* buffer;			// Buffering audio file
ndspWaveBuf waveBuf;
float mix[12];
unsigned long size; // Audio file size
unsigned long buf_pos;

char pcmout[4096];

void audio_init() {
	// Starting audio service
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
	ndspChnSetRate(0, 44100);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
    memset(&waveBuf,0,sizeof(waveBuf));
    memset(mix, 0, sizeof(mix));
    mix[0] = 1.0;
    mix[1] = 1.0;
    ndspChnSetMix(0, mix);
}

// Audio load/play
void audio_load_ogg(const char *audio) {
	/// Copied from ivorbisfile_example.c
	FILE *home = fopen(audio, "rb");
	OggVorbis_File vf;
	if (ov_open(home, &vf, NULL, 0)) {
		return;
	}
	size = (unsigned long)ov_pcm_total(&vf,-1);
	buffer = linearAlloc(size * 4);
	bool eof = false;
	int current_section;
	buf_pos = 0;
	while (!eof) {
		long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);

		if (ret == 0) {
			eof = true;
		} else if (ret < 0) {
			break;
		} else {
			memcpy(buffer+buf_pos, pcmout, ret);
			buf_pos += ret;
		}
	}
	ov_clear(&vf);
    waveBuf.data_vaddr = &buffer[0];
    waveBuf.nsamples = size;

    ndspChnWaveBufAdd(0, &waveBuf);
}

// Audio stop
void audio_stop(void) {
    ndspExit();
	// memset (buffer, 0, size);
	GSPGPU_FlushDataCache(buffer, size);
	linearFree(buffer);
}
