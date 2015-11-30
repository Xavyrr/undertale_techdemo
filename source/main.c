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
#include "friskBack1_png.h"
#include "friskBack2_png.h"
#include "friskBack3_png.h"
#include "friskBack4_png.h"
#include "friskFace1_png.h"
#include "friskFace2_png.h"
#include "friskFace3_png.h"
#include "friskFace4_png.h"
#include "friskLeft1_png.h"
#include "friskLeft2_png.h"
#include "friskRight1_png.h"
#include "friskRight2_png.h"
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

// Room collision
float room_x1 = 77;
float room_y1 = 60;
float room_x2 = 300;
float room_y2 = 188;

float speed = 0.5;

int textWidth = 0;
int textHeight = 0;

int prevTime = 0;
int currTime = 0;
float dt = 0;

// Data
sf2d_texture *curr_tex1;
sf2d_texture *curr_tex2;
sf2d_texture *curr_tex3;
sf2d_texture *curr_tex4;
sf2d_texture *curr_room;
sf2d_texture *tex_torielHouse1;
sftd_font *font;

//Rendering sprites and backgrounds
bool showEasterEggMessage = false;

void render () {
	sf2d_start_frame (GFX_TOP, GFX_LEFT);
	sf2d_draw_texture (curr_room, 40, 0);
	sf2d_draw_texture (curr_tex1, (int)player_x, (int)player_y);
	/*sleep (50);
	sf2d_draw_texture (curr_tex2, (int)player_x, (int)player_y);
	sleep (50);
	sf2d_draw_texture (curr_tex3, (int)player_x, (int)player_y);
	sleep (50);
	sf2d_draw_texture (curr_tex4, (int)player_x, (int)player_y);
	sleep (50);*/
	// Draw framerate
	sftd_draw_textf (font, 10, 10, RGBA8(255, 255, 255, 255), 12, "FPS: %f", sf2d_get_fps());
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

// Timer to make sure that the player movement is at the right speed
void timerStep () {
	prevTime = currTime;
	currTime = osGetTime();
	dt = currTime - prevTime;
	dt *= 0.2;
	if (dt < 0) dt = 0; // We don't want dt to be negative.
}

// Main part of the coding, where everything works (or not)
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
	sf2d_texture *tex_friskFace1 = sfil_load_PNG_buffer(friskFace1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskFace2 = sfil_load_PNG_buffer(friskFace2_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskFace3 = sfil_load_PNG_buffer(friskFace3_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskFace4 = sfil_load_PNG_buffer(friskFace4_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack1 = sfil_load_PNG_buffer(friskBack1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack2 = sfil_load_PNG_buffer(friskBack2_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack3 = sfil_load_PNG_buffer(friskBack3_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack4 = sfil_load_PNG_buffer(friskBack4_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft1 = sfil_load_PNG_buffer(friskLeft1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft2 = sfil_load_PNG_buffer(friskLeft2_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight1 = sfil_load_PNG_buffer(friskRight1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight2 = sfil_load_PNG_buffer(friskRight2_png, SF2D_PLACE_RAM);
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
		
		else if (kDown & KEY_SELECT) {
			showEasterEggMessage=true;
		}
		
		timerStep ();
		
		if (kHeld & KEY_UP) {
			player = 1;
			if (player_y >= room_y1) {
				player_y -= speed * dt;
			}
			else {
				player_y = room_y1 + 1;
			}
		}
		
		else if (kHeld & KEY_DOWN) {
			player = 0;
			if (player_y <= room_y2) {
				player_y += speed * dt;
			}
			else {
				player_y = room_y2 - 1;
			}
		}
		
		else if (kHeld & KEY_LEFT) {
			player = 2;
			if (player_x >= room_x1) {
				player_x -= speed * dt;
			}
			else {
				player_x = room_x1 + 1;
			}
		}
		
		else if (kHeld & KEY_RIGHT) {
			player = 3;
			if (player_x <= room_x2) {
				player_x += speed * dt;
			}
			else {
				player_x = room_x2 - 1;
			}
		}
		
		// Player sprites
		if (player == 0) {
			curr_tex1 = tex_friskFace1;
			curr_tex2 = tex_friskFace2;
			curr_tex3 = tex_friskFace3;
			curr_tex4 = tex_friskFace4;
		}
		
		else if (player == 1) {
			curr_tex1 = tex_friskBack1;
			curr_tex2 = tex_friskBack2;
			curr_tex3 = tex_friskBack3;
			curr_tex4 = tex_friskBack4;
		}
		
		else if (player == 2) {
			curr_tex1 = tex_friskLeft1;
			curr_tex2 = tex_friskLeft2;
			curr_tex3 = tex_friskLeft1;
			curr_tex4 = tex_friskLeft2;
		}
		
		else if (player == 3) {
			curr_tex1 = tex_friskRight1;
			curr_tex2 = tex_friskRight2;
			curr_tex3 = tex_friskRight1;
			curr_tex4 = tex_friskRight2;
		}
		
		// Localization/rooms
		if (room == 0) {
			curr_room = tex_torielHouse1;
		}
		
		render ();
	}
	
	// Free images/textures/fonts from memory
	sf2d_free_texture (tex_friskFace1);
	sf2d_free_texture (tex_friskBack1);
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