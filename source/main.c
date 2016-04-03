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

// Sound/Music stuff
u8* buffer;				// Buffering audio file
u32 size;				// Audio file size

// Audio load (play) and stop voids
static void audio_load(const char *audio);
static void audio_stop(void);

// Room variables
int room 		= 1;	// General info
int roomEnter 	= 0;	// Entrances

// Player variables
int player 		= 0;		// General info
int playerDir	= 0;	// Direction
float player_x;			// X coordinate
float player_y;			// Y coordinate
float screen_x;
float screen_y;
float hsp 		= 0;	// Horizontal speed
float vsp 		= 0;	// Vertical speed

// Text variables
int textWidth 	= 0;	// Width
int textHeight 	= 0;	// Height

// Timing variables
int prevTime 	= 0;	// Previous time
int currTime 	= 0;	// Current time
float dt 		= 0;	// Movement timing
double sprTimer = 0;	// Sprite timing

// sf2d_texture room_bg[6];

// Textures and fonts // TODO: Load these from files on the SD Card. Or something.
sf2d_texture 	*curr_tex;
sf2d_texture 	*tex_torielHouse1;
sf2d_texture 	*tex_torielHouse2;
sf2d_texture 	*tex_torielHouse3;
sf2d_texture 	*tex_torielHouse4;
sf2d_texture 	*tex_torielHouse5;
sf2d_texture 	*tex_torielHouse6;
sftd_font 		*font;

// Multidirectional array to store all player's walking textures
sf2d_texture* tex_arr_friskWalk[4][4];

char* friskFilenames[4][4] = {

	{"tex/friskRight0.png", "tex/friskRight1.png", "tex/friskRight0.png", "tex/friskRight1.png"}, 		// Right
	{"tex/friskFace0.png", "tex/friskFace1.png", "tex/friskFace2.png", "tex/friskFace3.png"},			// Down
	{"tex/friskLeft0.png", "tex/friskLeft1.png", "tex/friskLeft0.png", "tex/friskLeft1.png"},			// Left
	{"tex/friskBack0.png", "tex/friskBack1.png", "tex/friskBack2.png", "tex/friskBack3.png"}			// Up

};

struct room { // TODO: Move all of the room data into another file.
	sf2d_texture *tex;
	float x1;
	float y1;
	float x2;
	float y2;
	int tex_x;
	int tex_y;
	bool scrolling;
};

struct room rooms[4];

// struct room *curRoom = rooms[room];

// Constant variables for the player's walking textures
const int FRISK_RIGHT 	= 0;
const int FRISK_FORWARD = 1;
const int FRISK_LEFT 	= 2;
const int FRISK_BACK 	= 3;

// Easter Egg variables
bool easterEgg1 = false;

void init() {
	// Starting services
	sf2d_init();
	sf2d_set_vblank_wait(0);
	sftd_init();
	srvInit();
	aptInit();
	hidInit();
	//romfsInit();

	// Starting audio service
	csndInit();

	// Configuring the right font to use (8bitoperator), and its proprieties
	font = sftd_load_font_file("font/eightbit.ttf");

	// Configuring graphics in general (images, textures, etc)
	sf2d_set_clear_color(RGBA8 (0x00, 0x00, 0x00, 0xFF));
	FILE *log = fopen("UT.log","w");
	tex_torielHouse1 	= sfil_load_PNG_file("tex/torielHouse1.png", SF2D_PLACE_RAM);
	tex_torielHouse2 	= sfil_load_PNG_file("tex/torielHouse2.png", SF2D_PLACE_RAM);
	tex_torielHouse3 	= sfil_load_PNG_file("tex/torielHouse3.png", SF2D_PLACE_RAM);
	tex_torielHouse4 	= sfil_load_PNG_file("tex/torielHouse4.png", SF2D_PLACE_RAM);
	tex_torielHouse5 	= sfil_load_PNG_file("tex/torielHouse5.png", SF2D_PLACE_RAM);
	tex_torielHouse6 	= sfil_load_PNG_file("tex/torielHouse6.png", SF2D_PLACE_RAM);

	// Load Frisk textures
	// Loop over every element in tex_arr_friskWalk and load the PNG buffer
	// Because of C99, you have to declare the loop variables before the loop

	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			tex_arr_friskWalk[i][j] = sfil_load_PNG_file(friskFilenames[i][j], SF2D_PLACE_RAM);
		}
	}

	// Construct rooms. // TODO: Move to another file.
	// TODO: Move stuff around so that there is a room 0.
	rooms[1] = (struct room){
		tex_torielHouse1, // tex // TODO: Be able to reference the textures w/o initializing them first?
		77,   // x1
		60,   // y1
		305,  // x2
		188,  // y2
		40,   // tex_x
		0,    // tex_y
		false,// scrolling
	};
	rooms[2] = (struct room){
		tex_torielHouse2,
		60,
		69,
		320,
		190,
		40,
		0,
		false,
	};
	rooms[3] = (struct room){
		tex_torielHouse3,
		0,
		75,
		796,
		205,
		0,
		72,
		true,
	};

	// Play music
	audio_load("sound/music/home.bin");
}

void render() {
	// Start frame on the top screen
	sf2d_start_frame(GFX_TOP, GFX_LEFT);

	// Draw the background (or in this case, the room)
	sf2d_draw_texture(rooms[room].tex, rooms[room].tex_x + (int)screen_x, rooms[room].tex_y + (int)screen_y);

	// Draw the player's sprite
	sf2d_draw_texture(curr_tex, (int)player_x + (int)screen_x, (int)player_y + (int)screen_y);

	// End frame
	sf2d_end_frame();

	// If the easter egg variable is true, then activate it
	if (easterEgg1) {

		// Start frame on the bottom screen
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

		// Draw the easter egg
		sftd_draw_text(font, 10, 140,  RGBA8(255, 0, 0, 255), 16, "* You IDIOT.");
		sftd_draw_text(font, 10, 170,  RGBA8(255, 255, 255, 255), 16, "* Nah, this is just");
		sftd_draw_text(font, 10, 200,  RGBA8(255, 255, 255, 255), 16, "   a simple test.");

		// Debug stuff
		sftd_draw_textf(font, 10, 10, RGBA8(255, 0, 0, 255), 12, "FPS: %f", sf2d_get_fps());
		sftd_draw_textf(font, 10, 30, RGBA8(255, 0, 0, 255), 12, "Sprite Timer: %f", sprTimer);
		sftd_draw_textf(font, 10, 50, RGBA8(255, 0, 0, 255), 12, "Player X: %f, Y: %f", player_x, player_y);
		sftd_draw_textf(font, 10, 70, RGBA8(255, 0, 0, 255), 12, "Screen X: %f, Y: %f", screen_x, screen_y);

		// End frame
		sf2d_end_frame();

	};
}

// Timer for the player's speed
void timerStep() {
	// Set previous time as current time
	prevTime = currTime;

	// Set current time as the 3DS' OS RTC
	currTime = osGetTime();

	// Set and calculate the timer
	dt = currTime - prevTime;
	dt *= 0.15;

	// We don't want to dt to be negative.
	if (dt < 0) dt = 0;
}

// Main part of the coding, where everything works (or not)
int main(int argc, char **argv) {
	init();

	// Main loop
	while (aptMainLoop()) {

	// Verify button presses
	hidScanInput();

	// Unsigned variables for different types of button presses
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	// u32 kUp = hidKeysUp();

	// Exit homebrew
	if (kDown & KEY_START) {

		break;

	}

	// Activate first easter egg
	else if (kDown & KEY_SELECT) {

		easterEgg1 = true;

	}

	timerStep();

	// If no movement, set the sprite timer to 0
	if (kDown & KEY_UP || kDown & KEY_DOWN || kDown & KEY_LEFT || kDown & KEY_RIGHT) {

		sprTimer = 0;

	}

	// Reset horizontal and vertical speeds
	vsp = 0;
	hsp = 0;

	// Player movement (pretty easy to understand)
	if (kHeld & KEY_UP) {

		if (!(kHeld & KEY_DOWN)) {

			vsp 		= -.5;			// Vertical speed to negative .5
			playerDir 	= FRISK_BACK;	// Player direction = back

		}

	}

	if (kHeld & KEY_DOWN) {

		vsp 		= .5;				// Vertical speed to .5
		playerDir 	= FRISK_FORWARD;	// Player direction = up

	}

	if (kHeld & KEY_LEFT) {

		if (!(kHeld & KEY_RIGHT)) {

			hsp 		= -.5;			// Vertical speed to negative .5
			playerDir 	= FRISK_LEFT;	// Player direction = left

		}

	}

	if (kHeld & KEY_RIGHT) {

		hsp 		= .5;				// Vertical speed to .5
		playerDir 	= FRISK_RIGHT;		// Player direction = right

	}

	// Diagonal movement speed fix
	if (vsp != 0) {

		if (hsp != 0) {

			vsp *= .8;
			hsp *= .8;

		}

	}
	do {
		// Collision test before movement
		float tmp_x = player_x + hsp * dt;
		float tmp_y = player_y + vsp * dt;
		if (tmp_x >= rooms[room].x2 || \
			tmp_x <= rooms[room].x1 || \
			tmp_y >= rooms[room].y2 || \
			tmp_y <= rooms[room].y1) break; // Should probably just use a goto. Oh well.

		// Actual movement calculation

		if (rooms[room].scrolling) {
			if (tmp_x >= 300) {
				screen_x = 300 - tmp_x;
			}
			if (tmp_y <= 50) {
				screen_y = 50 - tmp_y;
			}
		} else screen_x = screen_y = 0;
		player_x = tmp_x;
		player_y = tmp_y;
	} while (0);


	// Player sprites
	if (hsp == 0 && vsp == 0) curr_tex = tex_arr_friskWalk[playerDir][0];

	else curr_tex = tex_arr_friskWalk[playerDir][(int)floor(sprTimer)];

	//Sprite animation timer
	sprTimer += (.03 * dt);

	while (sprTimer >= 4) {

		sprTimer -= 4;

	}

	// Localization/rooms
	if (room == 1) {

		if (roomEnter == 0) {

			player_x 	= 190;
			player_y 	= 160;

			roomEnter	= 255;

		}

		if (roomEnter == 1) {

			player_x 	= 78;
			player_y 	= 160;

			roomEnter 	= 255;

		}

		if (roomEnter == 2) {

			player_x 	= 304;
			player_y 	= 160;

			roomEnter 	= 255;

		}

		if (player_y >= 145 && player_y <= 195 && player_x <= 78 && playerDir == FRISK_LEFT) { // this needs work!

			room 		= 2;
			roomEnter 	= 0;

		}

		if (player_y >= 145 && player_y <= 195 && player_x >= 281 && playerDir == FRISK_RIGHT) { // this needs work!

			room 		= 3;
			roomEnter 	= 0;

		}

	}

	if (room == 2) {

		if (roomEnter == 0) {

			player_x 	= 319;
			player_y 	= 160;

			roomEnter 	= 255;

		}

		if (player_y >= 145 && player_y <= 195 && player_x >= 319 && playerDir == FRISK_RIGHT) { // this needs work!

			room = 1;
			roomEnter 	= 1;

		}

	}

	if (room == 3) {

		if (roomEnter == 0) {

			player_x 	= 41;
			player_y 	= 131;

			roomEnter 	= 255;

		}

		if (roomEnter == 1) {

			player_x 	= 338;
			player_y 	= 131;

			roomEnter 	= 255;

		}

		if (player_y >= 75 && player_y <= 205 && player_x <= 5 && playerDir == FRISK_LEFT) { // this needs work!

			room 		= 1;
			roomEnter 	= 2;

		}

	}


	render();

	// Swap sf2d framebuffers and wait for VBlank
	sf2d_swapbuffers();

}

	// Free images/textures/fonts from memory
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			sf2d_free_texture(tex_arr_friskWalk[i][j]);
		}
	}

	sf2d_free_texture(tex_torielHouse1);
	sf2d_free_texture(tex_torielHouse2);
	sf2d_free_texture(tex_torielHouse3);
	sf2d_free_texture(tex_torielHouse4);
	sf2d_free_texture(tex_torielHouse5);
	sf2d_free_texture(tex_torielHouse6);
	sftd_free_font(font);

	// Exit services
	sf2d_fini();
	sftd_fini();
	audio_stop();
	csndExit();
	hidExit();
	aptExit();
	srvExit();

	return 0;
}

// Audio load/play
void audio_load (const char *audio) {
	FILE *file = fopen(audio, "rb");
	fseek(file, 0, SEEK_END);
	off_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = linearAlloc (size);
	off_t bytesRead = fread(buffer, 1, size, file);
	fclose(file);
	csndPlaySound (8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 44100, 1, 0, buffer, buffer, size);
}

// Audio stop
void audio_stop (void) {

	csndExecCmds (true);
	CSND_SetPlayState (0x8, 0);
	// memset (buffer, 0, size);
	GSPGPU_FlushDataCache (buffer, size);
	linearFree (buffer);
}
