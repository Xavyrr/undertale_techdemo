// Include libraries
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "eightbit_ttf.h"

// Define the existence of images/textures
extern const struct {
 unsigned int width;
 unsigned int height;
 unsigned int bytes_per_pixel;
 unsigned char pixel_data [];
} friskFace;

extern const struct {
 unsigned int width;
 unsigned int height;
 unsigned int bytes_per_pixel;
 unsigned char pixel_data [];
} friskBack;

extern const struct {
 unsigned int width;
 unsigned int height;
 unsigned int bytes_per_pixel;
 unsigned char pixel_data [];
} friskLeft1;

extern const struct {
 unsigned int width;
 unsigned int height;
 unsigned int bytes_per_pixel;
 unsigned char pixel_data [];
} friskRight1;

extern const struct {
 unsigned int width;
 unsigned int height;
 unsigned int bytes_per_pixel;
 unsigned char pixel_data [];
} torielHouse1;

// Sound/Music stuff
u8* buffer;
u32 size;

static void audio_load(const char *audio);
static void audio_stop(void);

// Variables
int room = 0;
int player = 0;

int textWidth = 0;
int textHeight = 0;

int main (int argc, char **argv) {

	// Starting services
	sf2d_init ();
	sf2d_set_vblank_wait(0);
	sftd_init ();
	srvInit();
	aptInit();
	hidInit(NULL);
	
	// Audio service
	csndInit();
	
	// Configuring the right font to use (Roboto Thin), and its proprieties
	sftd_font *font = sftd_load_font_mem (eightbit_ttf, eightbit_ttf_size);
	
	// Configuring graphics in general (images, textures, etc)
	sf2d_set_clear_color (RGBA8 (0x40, 0x40, 0x40, 0xFF));
	sf2d_texture *tex_friskFace = sf2d_create_texture_mem_RGBA8(friskFace.pixel_data, friskFace.width, friskFace.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack = sf2d_create_texture_mem_RGBA8(friskBack.pixel_data, friskBack.width, friskBack.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft1 = sf2d_create_texture_mem_RGBA8(friskLeft1.pixel_data, friskLeft1.width, friskLeft1.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight1 = sf2d_create_texture_mem_RGBA8(friskRight1.pixel_data, friskRight1.width, friskRight1.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *tex_torielHouse1 = sf2d_create_texture_mem_RGBA8(torielHouse1.pixel_data, torielHouse1.width, torielHouse1.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	
	// Play music
	audio_load("sound/music/home.bin");
	
	// Main loop
	while (aptMainLoop ()) {
		
		// Verify button presses
		hidScanInput ();
		u32 kDown = hidKeysDown ();
		
		if (kDown & KEY_START) break;
		
		else if (kDown & KEY_UP) {
			player = 1;
		}
		
		else if (kDown & KEY_DOWN) {
			player = 0;
		}
		
		else if (kDown & KEY_LEFT) {
			player = 2;
		}
		
		else if (kDown & KEY_RIGHT) {
			player = 3;
		}
		
		// Player sprites and movements
		if (player == 0) {
			sf2d_start_frame (GFX_TOP, GFX_LEFT);
			sf2d_draw_texture (tex_torielHouse1, 0, 0);
			sf2d_draw_texture (tex_friskFace, 180, 80);
			sf2d_end_frame ();
		}
		
		else if (player == 1) {
			sf2d_start_frame (GFX_TOP, GFX_LEFT);
			sf2d_draw_texture (tex_torielHouse1, 0, 0);
			sf2d_draw_texture (tex_friskBack, 180, 80);
			sf2d_end_frame ();
		}
		
		else if (player == 2) {
			sf2d_start_frame (GFX_TOP, GFX_LEFT);
			sf2d_draw_texture (tex_torielHouse1, 0, 0);
			sf2d_draw_texture (tex_friskLeft1, 180, 80);
			sf2d_end_frame ();
		}
		
		else if (player == 3) {
			sf2d_start_frame (GFX_TOP, GFX_LEFT);
			sf2d_draw_texture (tex_torielHouse1, 0, 0);
			sf2d_draw_texture (tex_friskRight1, 180, 80);
			sf2d_end_frame ();
		}
		
		// Localization/rooms
		if (room == 0) {
		
		}
		
		// Swap sf2d framebuffers and wait for VBlank
		sf2d_swapbuffers ();
	}
	
	// Free images/textures/fonts from memory
	sf2d_free_texture (tex_friskFace);
	sf2d_free_texture (tex_friskBack);
	sf2d_free_texture (tex_friskLeft1);
	sf2d_free_texture (tex_friskRight1);
	sf2d_free_texture (tex_torielHouse1);
	sftd_free_font (font);

	// Exit services
	sf2d_fini ();
	sftd_fini ();
	audio_stop();
	csndExit();
	hidExit();
	aptExit();
	srvExit();

	return 0;
}

void audio_load (const char *audio) {
	FILE *file = fopen (audio, "rb");
	fseek (file, 0, SEEK_END);
	off_t size = ftell (file);
	fseek (file, 0, SEEK_SET);
	buffer = linearAlloc (size);
	off_t bytesRead = fread (buffer, 1, size, file);
	fclose (file);
	csndPlaySound (8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 44100, 1, 0, buffer, buffer, size);
	linearFree (buffer);
}
void audio_stop (void) {
	csndExecCmds (true);
	CSND_SetPlayState (0x8, 0);
	memset (buffer, 0, size);
	GSPGPU_FlushDataCache (NULL, buffer, size);
	linearFree (buffer);
}