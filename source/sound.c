#include "tremor/ivorbiscodec.h"
#include "tremor/ivorbisfile.h"
#include "sound.h"
#include <3ds.h>

#include <string.h>
#include <stdbool.h>

u8 *buffer;			// Buffering audio file
OggVorbis_File vf;
ndspWaveBuf waveBuf;
float mix[12];

unsigned long buf_samples; // Audio file size
unsigned long buf_pos = 0;
int section;
bool eof;

long mus_failure = 0;

void audio_init() {
	// Starting audio service
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
	ndspChnSetRate(0, 44100);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

	memset(&waveBuf,0,sizeof(waveBuf));
	waveBuf.looping = true;

	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(0, mix);
}

// Audio load/play
void audio_load_ogg(const char *audio) {
	/// Copied from ivorbisfile_example.c
	FILE *mus = fopen(audio, "rb");
	if (ov_open(mus, &vf, NULL, 0)) {
		return;
	}
	buf_samples = (unsigned long)ov_pcm_total(&vf,-1);
	buffer = linearAlloc(buf_samples * 4);
	waveBuf.data_vaddr = &buffer[0];
	waveBuf.nsamples = buf_samples;

	int i;

	for (i=0;i<6;++i) {
		audio_loop(4096);
	}

	ndspChnWaveBufAdd(0, &waveBuf);
}

void audio_loop(long size) {
	if (eof || mus_failure) return;

	{
		long tmp = buf_samples * 4 - buf_pos;
		size = (tmp < size) ? tmp : size; // min(tmp, size);
	}

	long amount = ov_read(&vf, buffer+buf_pos, size, &section);
	if (amount == 0) {
		ov_clear(&vf);
		eof = true;
	}

	else if (amount < 0) {
		mus_failure = amount;
		ndspChnReset(0);
		return;
	}

	else buf_pos += amount;
}

// Audio stop
void audio_stop(void) {
	ndspExit();
	// memset (buffer, 0, size);
	GSPGPU_FlushDataCache(buffer, buf_samples*4);
	linearFree(buffer);
}
