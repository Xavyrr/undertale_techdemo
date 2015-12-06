// Include libraries
#include <stdbool.h>
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
#include "friskBack0_png.h"
#include "friskFace1_png.h"
#include "friskFace2_png.h"
#include "friskFace3_png.h"
#include "friskFace0_png.h"
#include "friskLeft1_png.h"
#include "friskLeft0_png.h"
#include "friskRight1_png.h"
#include "friskRight0_png.h"
#include "torielHouse1_png.h"
#include "torielHouse2_png.h"




// Sound/Music stuff
u8* buffer;
u32 size;
static void audio_load(const char *audio);
static void audio_stop(void);

// "int" Variables
int room = 1;
int roomEnter = 0;
int player = 0;
int playerDir = 0; //direction player is facing
//	0 = right, 1 = up, 2 = left, 3 = down
int textWidth = 0;
int textHeight = 0;
int prevTime = 0;
int currTime = 0;

// "float" Variables
float player_x;
float player_y;
float room_x1;
float room_y1;
float room_x2;
float room_y2;
//float speed = 0.5; // replaced with hsp and vsp
float hsp = 0; //player horizontal speed
float vsp = 0; //player vertical speed
float dt = 0;
double sprTimer = 0;

// Data
sf2d_texture *curr_tex;
sf2d_texture *curr_room;
sf2d_texture *tex_torielHouse1;
sf2d_texture *tex_torielHouse2;
sftd_font *font;

sf2d_texture* tex_arr_friskWalk [4][4]; //multidimensional array to store all textures

const u8* friskFilenames [4][4]= { //multidimensional array to store all the filenames
{friskRight0_png,friskRight1_png,friskRight0_png,friskRight1_png}, //order is right->up->left->down like the rest of the program
{friskFace0_png,friskFace1_png,friskFace2_png,friskFace3_png,},
{friskLeft0_png,friskLeft1_png,friskLeft0_png,friskLeft1_png},
{friskBack0_png,friskBack1_png,friskBack2_png,friskBack3_png}
};

const int FRISK_RIGHT = 0;
const int FRISK_FORWARD = 1;
const int FRISK_LEFT = 2;
const int FRISK_BACK = 3;

//Rendering sprites and backgrounds
bool showEasterEggMessage = false;


void render () {
	sf2d_start_frame (GFX_TOP, GFX_LEFT);
	sf2d_draw_texture (curr_room, 40, 0);
	sf2d_draw_texture (curr_tex, (int)player_x, (int)player_y);
	/*sleep (50); // Sleep commands for some reason, don't work at all
	sf2d_draw_texture (curr_tex2, (int)player_x, (int)player_y);
	sleep (50);
	sf2d_draw_texture (curr_tex3, (int)player_x, (int)player_y);
	sleep (50);
	sf2d_draw_texture (curr_tex4, (int)player_x, (int)player_y);
	sleep (50);*/
	sf2d_end_frame ();

	if (showEasterEggMessage) {
		sf2d_start_frame (GFX_BOTTOM, GFX_LEFT);
		sftd_draw_text (font, 10, 140,  RGBA8(255, 0, 0, 255), 16, "* You IDIOT.");
		sftd_draw_text (font, 10, 170,  RGBA8(255, 255, 255, 255), 16, "* Nah, this is just");
		sftd_draw_text (font, 10, 200,  RGBA8(255, 255, 255, 255), 16, "   a simple test.");
		sftd_draw_textf (font, 10, 10, RGBA8(255, 0, 0, 255), 12, "FPS: %f", sf2d_get_fps());
		sftd_draw_textf (font, 10, 30, RGBA8(255, 0, 0, 255), 12, "Sprite Timer: %f", sprTimer);
		sf2d_end_frame ();
	};
	
	//sftd_draw_text (font, 10, 20, RGBA8(255, 255, 255, 255), 12, "sprTimer: " + string(sprTimer));
	//sf2d_end_frame ();

	// Swap sf2d framebuffers and wait for VBlank
	sf2d_swapbuffers ();
}

// Timer to make sure that the player movement is at the right speed
void timerStep () {
	prevTime = currTime;
	currTime = osGetTime();
	dt = currTime - prevTime;
	dt *= 0.15;
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
	tex_torielHouse1 = sfil_load_PNG_buffer(torielHouse1_png, SF2D_PLACE_RAM);
	tex_torielHouse2 = sfil_load_PNG_buffer(torielHouse2_png, SF2D_PLACE_RAM);

	// Load Frisk textures
	// loop over every element in tex_arr_friskWalk and load the PNG buffer
	// for some reason here you have to declare the loop variables before the loop
	int i;
	int j;

	for (i=0;i<4;i++){ 
		for (j=0;j<4;j++){
			tex_arr_friskWalk[i][j] = sfil_load_PNG_buffer(friskFilenames[i][j], SF2D_PLACE_RAM);
		}
	}


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

		if (kDown & KEY_UP || kDown & KEY_DOWN || kDown & KEY_LEFT || kDown & KEY_RIGHT) sprTimer = 0;

		//Key presses set speed
		vsp = 0; //reset hsp and vsp just in case...
		hsp = 0;
		if (kHeld & KEY_UP) {
			if (!(kHeld & KEY_DOWN)) {
				vsp = -.5;
				playerDir = 1;
			}
		}
		if (kHeld & KEY_DOWN) {
			vsp = .5;
			playerDir = 3;
		}
		if (kHeld & KEY_LEFT) {
			if (!(kHeld & KEY_RIGHT)) {
				hsp = -.5;
				playerDir = 2;
			}
		}
		if (kHeld & KEY_RIGHT) {
			hsp = .5;
			playerDir = 0;
		}
		//diagonal speed fix
		//iirc Undertale doesn't have diagonal movement,
		//so I might remove this later and change the
		//movement code to reflect that.
		if (vsp != 0) {
				if (hsp != 0) {
					vsp *= .7;
					hsp *= .7;
				}
		}
		// Collision test BEFORE movement
		if ((player_x + hsp) >= room_x2) {
			hsp = 0;
		}
		if ((player_x + hsp) <= room_x1) {
			hsp = 0;
		}
		if ((player_y + vsp) >= room_y2) {
			vsp = 0;
		}
		if ((player_y + vsp) <= room_y1) {
			vsp = 0;
		}

		// Actual movement calculation
		player_x += hsp * dt;
		player_y += vsp * dt;
		// Player sprites
		if (playerDir == 0) {
			if (hsp == 0) {
				curr_tex = tex_arr_friskWalk[FRISK_RIGHT][0];
			}
			else {
				curr_tex = tex_arr_friskWalk[FRISK_RIGHT][(int)floor(sprTimer)];
			}
		}
		if (playerDir == 1) {
			if (vsp == 0) {
				curr_tex = tex_arr_friskWalk[FRISK_BACK][0];
			}
			else {
				curr_tex = tex_arr_friskWalk[FRISK_BACK][(int)floor(sprTimer)];
			}
		}
		if (playerDir == 2) {
			if (hsp == 0) {
				curr_tex = tex_arr_friskWalk[FRISK_LEFT][0];
			}
			else {
				curr_tex = tex_arr_friskWalk[FRISK_LEFT][(int)floor(sprTimer)];
			}
		}
		if (playerDir == 3) {
			if (vsp == 0) {
				curr_tex = tex_arr_friskWalk[FRISK_FORWARD][0];
			}
			else {
				curr_tex = tex_arr_friskWalk[FRISK_FORWARD][(int)floor(sprTimer)];
			}
		}
		//Sprite animation timer
		sprTimer += (.03 * dt);
		while(sprTimer >= 4) {
			sprTimer -= 4;
		}
		// Localization/rooms
		if (room == 1) {
			if (roomEnter == 0) {
				curr_room = tex_torielHouse1;
				player_x = 190;
				player_y = 160;
				room_x1 = 77;
				room_y1 = 60;
				room_x2 = 305;
				room_y2 = 188;
				roomEnter = 255;
			}

			if (roomEnter == 1) {
				curr_room = tex_torielHouse1;
				player_x = 80;
				player_y = 160;
				room_x1 = 77;
				room_y1 = 60;
				room_x2 = 305;
				room_y2 = 188;
				roomEnter = 255;
			}

			if (player_y >= 145 && player_y <= 195 && player_x <= 80 && playerDir == 2) { // this needs work!
				room = 2;
				roomEnter = 0;
			}
		}
		if (room == 2) {
			if (roomEnter == 0) {
				curr_room = tex_torielHouse2;
				player_x = 315;
				player_y = 160;
				room_x1 = 60;
				room_y1 = 69;
				room_x2 = 320;
				room_y2 = 190;
				roomEnter = 255;
			}

			if (player_y >= 145 && player_y <= 195 && player_x >= 317 && playerDir == 0) { // this needs work!
				room = 1;
				roomEnter = 1;
			}
		}

		render ();
	}

	// Free images/textures/fonts from memory
	//int i,j;
	for(i=0;i<4;i++){ 
		for(j=0;j<4;j++){
			sf2d_free_texture(tex_arr_friskWalk[i][j]);
		}
	}

	sf2d_free_texture (tex_torielHouse1);
	sf2d_free_texture (tex_torielHouse2);

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
