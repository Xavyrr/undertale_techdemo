// Include libraries
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "eightbit_ttf.h"

// Load images
#include "friskBack_png.h"
#include "friskFace_png.h"
#include "friskLeft1_png.h"
#include "friskRight1_png.h"
#include "torielHouse1_png.h"

// Sound/Music stuff
u8* buffer;
u32 size;

static void audio_load(const char *audio);
static void audio_stop(void);

// Variables
int room = 0;

int player = 0;
float player_x = 190;
float player_y = 160;

float speed = 0.5;

int textWidth = 0;
int textHeight = 0;

// Data
sf2d_texture *curr_tex;
sf2d_texture *tex_torielHouse1;
sftd_font *font;

//Rendering sprites and backgrounds
bool showEasterEggMessage = false;

void render () {
	sf2d_start_frame (GFX_TOP, GFX_LEFT);
	sf2d_draw_texture (tex_torielHouse1, 40, 0);
	sf2d_draw_texture (curr_tex, (int)player_x, (int)player_y);
	sf2d_end_frame ();

	if (showEasterEggMessage) {
		sf2d_start_frame (GFX_BOTTOM, GFX_LEFT);
		sftd_draw_text (font, 10, 140,  RGBA8(255, 0, 0, 255), 20, "* You IDIOT.");
		sftd_draw_text (font, 10, 170,  RGBA8(255, 255, 255, 255), 20, "* Nah, this is just");
		sftd_draw_text (font, 10, 200,  RGBA8(255, 255, 255, 255), 20, "   a simple test.");
		sf2d_end_frame ();
	};
		
	// Swap sf2d framebuffers and wait for VBlank
	sf2d_swapbuffers ();
}

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
	
	// Configuring the right font to use (8bitoperator), and its proprieties
	font = sftd_load_font_mem (eightbit_ttf, eightbit_ttf_size);
	
	// Configuring graphics in general (images, textures, etc)
	sf2d_set_clear_color (RGBA8 (0x00, 0x00, 0x00, 0xFF));
	sf2d_texture *tex_friskFace = sfil_load_PNG_buffer(friskFace_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack = sfil_load_PNG_buffer(friskBack_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft1 = sfil_load_PNG_buffer(friskLeft1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight1 = sfil_load_PNG_buffer(friskRight1_png, SF2D_PLACE_RAM);
	tex_torielHouse1 = sfil_load_PNG_buffer(torielHouse1_png, SF2D_PLACE_RAM);
	
	// Play music
	audio_load("sound/music/home.bin");
	
	// Main loop
	while (aptMainLoop ()) {
		
		// Verify button presses
		hidScanInput ();
		u32 kDown = hidKeysDown ();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();
		
		if (kDown & KEY_START) break;
		
		if (kDown & KEY_SELECT) {
			showEasterEggMessage=true;
		}
		
		else if (kHeld & KEY_UP) {
			player = 1;
			player_y -= speed;
		}
		
		else if (kHeld & KEY_DOWN) {
			player = 0;
			player_y += speed;
		}
		
		else if (kHeld & KEY_LEFT) {
			player = 2;
			player_x -= speed;
		}
		
		else if (kHeld & KEY_RIGHT) {
			player = 3;
			player_x += speed;
		}
		
		// Player sprites
		if (player == 0) {
			curr_tex = tex_friskFace;
		}
		
		else if (player == 1) {
			curr_tex = tex_friskBack;
		}
		
		else if (player == 2) {
			curr_tex = tex_friskLeft1;
		}
		
		else if (player == 3) {
			curr_tex = tex_friskRight1;
		}
		
		// Localization/rooms
		if (room == 0) {
		
		}

		render();
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
}

void audio_stop (void) {
	csndExecCmds (true);
	CSND_SetPlayState (0x8, 0);
	memset (buffer, 0, size);
	GSPGPU_FlushDataCache (NULL, buffer, size);
	linearFree (buffer);
}
