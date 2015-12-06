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
#include "torielHouse31_png.h"
#include "torielHouse32_png.h"

// Sound/Music stuff
u8* buffer;
u32 size;
static void audio_load(const char *audio);
static void audio_stop(void);

// Room Variables
int room = 1;
int roomEnter = 0;
float room_x1;
float room_y1;
float room_x2;
float room_y2;

// Player Variables
int player = 0; // Player's info in general
int playerDir = 0; // Direction the player is facing (0 = Right, 1 = Up, 2 = Left, 3 = Down)
float player_x; // Player's X coordinates
float player_y; // Player's Y coordinates
float hsp = 0; // Player's horizontal speed
float vsp = 0; // Player's vertical speed

// Text Variables
int textWidth = 0;
int textHeight = 0;

// Timing Variables
int prevTime = 0;
int currTime = 0;
float dt = 0;
double sprTimer = 0;

// Easter Egg Variable
bool EasterEgg1 = false;

// Data for images/textures/sprites/fonts (refactored)
sf2d_texture *curr_tex;
sf2d_texture *curr_room;
sf2d_texture *tex_torielHouse1;
sf2d_texture *tex_torielHouse2;
sftd_font *font;

// Rendering the whole game (sprites, text, backgrounds, images, etc)
void render () {

	// Start a frame on the top screen
	sf2d_start_frame (GFX_TOP, GFX_LEFT);
	
	// Render the background (room)
	sf2d_draw_texture (curr_room, 40, 0);
	
	// Render the player (direction and speed)
	sf2d_draw_texture (curr_tex, (int)player_x, (int)player_y);
	
	// End frame
	sf2d_end_frame ();

	// If the EasterEgg1 variable is true, then activate it
	if (EasterEgg1) {
		
		// Start a frame on the bottom screen
		sf2d_start_frame (GFX_BOTTOM, GFX_LEFT);
		
		// Draw the easter egg
		sftd_draw_text (font, 10, 140,  RGBA8(255, 0, 0, 255), 16, "* You IDIOT.");
		sftd_draw_text (font, 10, 170,  RGBA8(255, 255, 255, 255), 16, "* Nah, this is just");
		sftd_draw_text (font, 10, 200,  RGBA8(255, 255, 255, 255), 16, "   a simple test.");
		
		// Draw debug stuff (FPS, Sprite Timer, etc)
		sftd_draw_textf (font, 10, 10, RGBA8(255, 0, 0, 255), 12, "FPS: %f", sf2d_get_fps());
		sftd_draw_textf (font, 10, 30, RGBA8(255, 0, 0, 255), 12, "Sprite Timer: %f", sprTimer);
		
		// End frame
		sf2d_end_frame ();
	};

	// Swap sf2d framebuffers and wait for VBlank
	sf2d_swapbuffers ();
	
}

// Timer for the player's speed
void timerStep () {
	
	// Set previous time as current time
	prevTime = currTime;
	
	// Set current time as the 3DS' RTC time
	currTime = osGetTime();
	
	// Calculate the timer
	dt = currTime - prevTime;
	dt *= 0.15;
	
	// Avoiding the timer to become negative
	if (dt < 0) {
		
		dt = 0;
		
	}
	
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
	sf2d_texture *tex_friskFace0 = sfil_load_PNG_buffer(friskFace0_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack1 = sfil_load_PNG_buffer(friskBack1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack2 = sfil_load_PNG_buffer(friskBack2_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack3 = sfil_load_PNG_buffer(friskBack3_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskBack0 = sfil_load_PNG_buffer(friskBack0_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft1 = sfil_load_PNG_buffer(friskLeft1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskLeft0 = sfil_load_PNG_buffer(friskLeft0_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight1 = sfil_load_PNG_buffer(friskRight1_png, SF2D_PLACE_RAM);
	sf2d_texture *tex_friskRight0 = sfil_load_PNG_buffer(friskRight0_png, SF2D_PLACE_RAM);
	tex_torielHouse1 = sfil_load_PNG_buffer(torielHouse1_png, SF2D_PLACE_RAM);
	tex_torielHouse2 = sfil_load_PNG_buffer(torielHouse2_png, SF2D_PLACE_RAM);

	// Play music
	audio_load("sound/music/home.bin");

	// Main loop
	while (aptMainLoop ()) {
		
		// Verify button presses
		hidScanInput ();
		
		// Create unsigned 32-bit variables for types of button presses
		u32 kDown = hidKeysDown ();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();
		
		// If the START button is pressed down, then exit the demo to the homebrew menu
		if (kDown & KEY_START) {
			
			break;
			
		}
		
		// If the SELECT button is pressed down, then activate the first easter egg
		else if (kDown & KEY_SELECT) {
			
			EasterEgg1 = true;
			
		}
		
		// Running the timer before using it
		timerStep ();
		
		// Key presses set speed
		if (kDown & KEY_UP) sprTimer = 0;
		if (kDown & KEY_DOWN) sprTimer = 0;
		if (kDown & KEY_LEFT) sprTimer = 0;
		if (kDown & KEY_RIGHT) sprTimer = 0;
		
		// Reseting hsp and vsp variables
		vsp = 0;
		hsp = 0;
		
		// If the UP arrow from the D-Pad is pressed, then move the player up
		if (kHeld & KEY_UP) {
			
			// If the DOWN arrow from the D-Pad isn't interfering, then continue with the movement code
			if (! (kHeld & KEY_DOWN)) {
				
				// Set vertical speed to negative .5
				vsp = -.5;
				
				// Set player's direction to 1 (up)
				playerDir = 1;
				
			}
			
		}
		
		// If the DOWN arrow from the D-Pad is pressed, then move the player down
		if (kHeld & KEY_DOWN) {
			
			// Set vertical speed to .5
			vsp = .5;
			
			// Set the player's direction to 3 (down)
			playerDir = 3;
			
		}
		
		// If the LEFT arrow from the D-Pad is pressed, then move the player left
		if (kHeld & KEY_LEFT) {
			
			// If the RIGHT arrow from the D-Pad isn't interfering, then continue with the movement code
			if (!(kHeld & KEY_RIGHT)) {
				
				// Set horizontal movement to negative .5
				hsp = -.5;
				
				// Set the player's direction to 2 (left)
				playerDir = 2;
				
			}
			
		}
		
		// If the RIGHT arrow from the D-Pad is pressed, then move the player right
		if (kHeld & KEY_RIGHT) {
			
			// Set horizontal movement to .5
			hsp = .5;
			
			// Set the player's direction to 0 (right)
			playerDir = 0;
			
		}
		
		
		// Diagonal speed fix
		if (vsp != 0) {
			
			if (hsp != 0) {
				
				vsp *= .8;
				hsp *= .8;
				
			}
			
		}
		
		
		// Collision test before movement (easy to understand)
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
		
		/*
			Pretty easy to explain how the sprites and
			animations works:
			
			The code will look for what variable is
			currently playerDir (ex: 3 = down).
			After that, it will see what is the
			current speed of the player. If it's
			0 (colliding with a wall or just standing),
			no animation will happen. But, if the player
			is walking (or its speed is different than
			0), then the animation will come up.
			
			Sprite and animation code below:
		*/
		if (playerDir == 0) {
			
			if (hsp == 0) {
				curr_tex = tex_friskRight0;
			}
			
			else {
				
				if (sprTimer >= 0) {
					curr_tex = tex_friskRight0;
				}
				if (sprTimer >= 1) {
					curr_tex = tex_friskRight1;
				}
				if (sprTimer >= 2) {
					curr_tex = tex_friskRight0;
				}
				if (sprTimer >= 3) {
					curr_tex = tex_friskRight1;
				}
				
			}
			
		}
		
		if (playerDir == 1) {
			
			if (vsp == 0) {
				curr_tex = tex_friskBack0;
			}
			
			else {
				
				if (sprTimer >= 0) {
					curr_tex = tex_friskBack0;
				}
				if (sprTimer >= 1) {
					curr_tex = tex_friskBack1;
				}
				if (sprTimer >= 2) {
					curr_tex = tex_friskBack2;
				}
				if (sprTimer >= 3) {
					curr_tex = tex_friskBack3;
				}
				
			}
			
		}
		
		if (playerDir == 2) {
			
			if (hsp == 0) {
				curr_tex = tex_friskLeft0;
			}
			
			else {
			
				if (sprTimer >= 0) {
					curr_tex = tex_friskLeft0;
				}
				if (sprTimer >= 1) {
					curr_tex = tex_friskLeft1;
				}
				if (sprTimer >= 2) {
					curr_tex = tex_friskLeft0;
				}
				if (sprTimer >= 3) {
					curr_tex = tex_friskLeft1;
				}
				
			}
			
		}
		
		if (playerDir == 3) {
			
			if (vsp == 0) {
				curr_tex = tex_friskFace0;
			}
			
			else {
				
				if (sprTimer >= 0) {
					curr_tex = tex_friskFace0;
				}
				if (sprTimer >= 1) {
					curr_tex = tex_friskFace1;
				}
				if (sprTimer >= 2) {
					curr_tex = tex_friskFace2;
				}
				if (sprTimer >= 3) {
					curr_tex = tex_friskFace3;
				}
				
			}
			
		}
		
		//Sprite animation timer
		sprTimer += (.03 * dt);
		
		if (sprTimer >= 4) {
			
			sprTimer = 0;
			
		}
		
		// Localization/rooms (currently a mess, will be remade)
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
			
			if (player_y >= 145 && player_y <= 195 && player_x <= 80 && playerDir == 2) { // This needs work (obviously)!
				
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
			
			if (player_y >= 145 && player_y <= 195 && player_x >= 317 && playerDir == 0) { // This needs work (obviously)!
				
				room = 1;
				roomEnter = 1;
				
			}
			
		}
		
		render ();
	}

	// Free images/textures/fonts from memory
	sf2d_free_texture (tex_friskFace1);
	sf2d_free_texture (tex_friskFace2);
	sf2d_free_texture (tex_friskFace3);
	sf2d_free_texture (tex_friskFace0);
	sf2d_free_texture (tex_friskBack1);
	sf2d_free_texture (tex_friskBack2);
	sf2d_free_texture (tex_friskBack3);
	sf2d_free_texture (tex_friskBack0);
	sf2d_free_texture (tex_friskLeft1);
	sf2d_free_texture (tex_friskLeft0);
	sf2d_free_texture (tex_friskRight1);
	sf2d_free_texture (tex_friskRight0);
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
