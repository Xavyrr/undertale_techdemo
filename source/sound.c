#include "tremor/ivorbiscodec.h"
#include "tremor/ivorbisfile.h"
#include "sound.h"
#include <3ds.h>

#include <string.h>
#include <stdbool.h>

u8* buffer;			// Buffering audio file
unsigned long size; // Audio file size
unsigned long buf_pos;

char pcmout[4096];

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
    ndspChnSetFormat(8, NDSP_FORMAT_STEREO_PCM16);
	csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 44100, 1, 0, buffer, buffer, size*4);
}

// Audio stop
void audio_stop(void) {
	csndExecCmds(true);
	CSND_SetPlayState(0x8, 0);
	// memset (buffer, 0, size);
	GSPGPU_FlushDataCache(buffer, size);
	linearFree(buffer);
}
