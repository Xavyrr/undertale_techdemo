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

#include "sound.h"

typedef struct position {
    float x;
    float y;
} position;

// Room variables
int room      = 1; // General info
int roomEnter = 0; // Entrances

// Player variables
int player    = 0; // General info
int playerDir = 0; // Direction

position player_pos = {0, 0};
position camera_pos = {0, 0};

float hsp = 0; // Horizontal speed
float vsp = 0; // Vertical speed

// Text variables
int textWidth  = 0; // Width
int textHeight = 0; // Height

// Timing variables
int    prevTime = 0; // Previous time
int    currTime = 0; // Current time
float  dt       = 0; // Movement timing
double sprTimer = 0; // Sprite timing

// sf2d_texture room_bg[6];

// Textures and fonts
sf2d_texture *curr_tex;
sf2d_texture *tex_torielHouse1;
sf2d_texture *tex_torielHouse2;
sf2d_texture *tex_torielHouse3;
sf2d_texture *tex_torielHouse4;
sf2d_texture *tex_torielHouse5;
sf2d_texture *tex_torielHouse6;
sftd_font    *font;

// Multidirectional array to store all player's walking textures
sf2d_texture* tex_arr_friskWalk[4][4];

char* friskFilenames[4][4] = {
    {"tex/friskRight0.png", "tex/friskRight1.png", "tex/friskRight0.png", "tex/friskRight1.png"},// Right
    {"tex/friskFace0.png", "tex/friskFace1.png", "tex/friskFace2.png", "tex/friskFace3.png"}, // Down
    {"tex/friskLeft0.png", "tex/friskLeft1.png", "tex/friskLeft0.png", "tex/friskLeft1.png"}, // Left
    {"tex/friskBack0.png", "tex/friskBack1.png", "tex/friskBack2.png", "tex/friskBack3.png"}  // Up
};

struct exit {
    int room_id;
    position entrance;
    position collision[2];
};

struct room { // TODO: Move all of the room data into another file.
    sf2d_texture *tex;
    position collision[2];
    position tex_pos;
    bool scrolling;
    unsigned int num_exit;
    struct exit *exits;
};

struct room rooms[4];

// struct room *curRoom = rooms[room];

// Constant variables for the player's walking textures
const int FRISK_RIGHT   = 0;
const int FRISK_FORWARD = 1;
const int FRISK_LEFT    = 2;
const int FRISK_BACK    = 3;

// Easter Egg variables
bool easterEgg  = false;
int  easterPage = 0;
#define MAX_PAGE 1

void init() {
    // Starting services
    sf2d_init();
    sf2d_set_vblank_wait(0);
    sftd_init();
    srvInit();
    aptInit();
    hidInit();
    audio_init();
    //romfsInit();

    // Configuring the right font to use (8bitoperator), and its proprieties
    font = sftd_load_font_file("font/eightbit.ttf");

    // Configuring graphics in general (images, textures, etc)
    sf2d_set_clear_color(RGBA8 (0x00, 0x00, 0x00, 0xFF));
    tex_torielHouse1 = sfil_load_PNG_file("tex/torielHouse1.png", SF2D_PLACE_RAM);
    tex_torielHouse2 = sfil_load_PNG_file("tex/torielHouse2.png", SF2D_PLACE_RAM);
    tex_torielHouse3 = sfil_load_PNG_file("tex/torielHouse3.png", SF2D_PLACE_RAM);
    tex_torielHouse4 = sfil_load_PNG_file("tex/torielHouse4.png", SF2D_PLACE_RAM);
    tex_torielHouse5 = sfil_load_PNG_file("tex/torielHouse5.png", SF2D_PLACE_RAM);
    tex_torielHouse6 = sfil_load_PNG_file("tex/torielHouse6.png", SF2D_PLACE_RAM);

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
        { // collision
            {77,  60}, // pos0
            {305, 188} // pos1
        },
        {40, 0}, // tex_pos
        false, // scrolling
        3, // num_exits
    };
    rooms[1].exits = malloc(rooms[1].num_exit * sizeof(struct exit));
    rooms[1].exits[0] = (struct exit){ // HACK: Player spawns at 0, 0 by default. Rather than fixing this, just teleport!
        1,
        {190, 160},
        {
            {-1, -1},
            {1, 1}
        }
    };
    rooms[1].exits[1] = (struct exit){ // 1
        2, // room_id
        {319, 160}, // entrance
        { // collision
            {0, 145}, // pos1
            {78,  195}  // pos2
        }
    };

    rooms[1].exits[2] = (struct exit){
        3,
        {41, 131},
        {
            {281, 145},
            {300, 195}
        }
    };

    rooms[2] = (struct room){
        tex_torielHouse2,
        {
            {60,  69},
            {320, 190}
        },
        {40, 0},
        false,
        1,
    };
    rooms[2].exits = malloc(rooms[2].num_exit * sizeof(struct exit));

    rooms[2].exits[0] = (struct exit){
        1,
        {78, 160},
        {
            {319, 145},
            {400, 195}
        }
    };

    rooms[3] = (struct room){
        tex_torielHouse3,
        {
            {0, 130},
            {710, 175}
        },
        {0, 72},
        true,
        1,
    };
    rooms[3].exits = malloc(rooms[3].num_exit * sizeof(struct exit));

    rooms[3].exits[0] = (struct exit){
        1,
        {280, 160},
        {
            {-5, 75},
            {5, 205}
        }
    };

    // Play music
    audio_load_ogg("sound/music/house1.ogg");
}

void render() {
    // Start frame on the top screen
    sf2d_start_frame(GFX_TOP, GFX_LEFT);

    // Draw the background (or in this case, the room)
    sf2d_draw_texture(rooms[room].tex, rooms[room].tex_pos.x + (int)camera_pos.x,
                                       rooms[room].tex_pos.y + (int)camera_pos.y);

    // Draw the player's sprite
    sf2d_draw_texture(curr_tex, (int)player_pos.x + (int)camera_pos.x,
                                (int)player_pos.y + (int)camera_pos.y);

    // End frame
    sf2d_end_frame();

    // Start frame on the bottom screen
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

    // If the easter egg variable is true, then activate it
    if (easterEgg) {
        // Draw the easter egg
        sftd_draw_text(font, 10, 140,  RGBA8(255, 0, 0, 255), 16, "* You IDIOT.");
        sftd_draw_text(font, 10, 170,  RGBA8(255, 255, 255, 255), 16, "* Nah, this is just");
        sftd_draw_text(font, 10, 200,  RGBA8(255, 255, 255, 255), 16, "   a simple test.");
        int y = -10;
        // Debug stuff
        switch (easterPage) {
            case 0:
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 0, 0, 255), 12, "FPS: %f", sf2d_get_fps());
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 0, 0, 255), 12, "Sprite Timer: %f", sprTimer);
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 255, 255, 255), 12, "Player X: %f, Y: %f", player_pos.x, player_pos.y);
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 255, 255, 255), 12, "Screen X: %f, Y: %f", camera_pos.x, camera_pos.y);
                break;
            case 1:
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 0, 0, 255), 12, "Samples: %lu", buf_samples);
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 255, 255, 255), 12, "Buffer Position: %lu", buf_pos);
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 0, 0, 255), 12, "Amount: %li", mus_failure);
                sftd_draw_textf(font, 10, y+=20, RGBA8(255, 0, 0, 255), 12, "EOF: %d", eof);
                break;
        }
    };
    // End frame
    sf2d_end_frame();
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
        audio_loop();

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
            easterEgg = !easterEgg;
        }

        // Change pages for the easterEgg/debug menu.
        else if (kDown & KEY_R) {
            if (++easterPage > MAX_PAGE) easterPage = 0;
        }
        else if (kDown & KEY_L) {
            if (--easterPage < 0) easterPage = MAX_PAGE;
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
                vsp = -.5; // Vertical speed to negative .5
                playerDir = FRISK_BACK; // Player direction = back
            }
        }

        if (kHeld & KEY_DOWN) {
            vsp = .5; // Vertical speed to .5
            playerDir = FRISK_FORWARD; // Player direction = up
        }

        if (kHeld & KEY_LEFT) {
            if (!(kHeld & KEY_RIGHT)) {
                hsp = -.5; // Vertical speed to negative .5
                playerDir = FRISK_LEFT; // Player direction = left
            }
        }

        if (kHeld & KEY_RIGHT) {
            hsp = .5; // Vertical speed to .5
            playerDir = FRISK_RIGHT; // Player direction = right
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
            float tmp_x = player_pos.x + hsp * dt;
            float tmp_y = player_pos.y + vsp * dt;
            if (tmp_x >= rooms[room].collision[1].x || \
                tmp_x <= rooms[room].collision[0].x || \
                tmp_y >= rooms[room].collision[1].y || \
                tmp_y <= rooms[room].collision[0].y) break; // Should probably just use a goto. Oh well.

            // Actual movement calculation
            if (rooms[room].scrolling) { // TODO: Preform scrolling that stays still.
                if (tmp_x >= 300) {
                    camera_pos.x = 300 - tmp_x;
                }
                if (tmp_y <= 50) {
                    camera_pos.y = 50 - tmp_y;
                }
            } else camera_pos.x = camera_pos.y = 0;

            player_pos = (struct position){tmp_x, tmp_y};
        } while (0);


        // Player sprites
        if (hsp == 0 && vsp == 0) curr_tex = tex_arr_friskWalk[playerDir][0];

        else curr_tex = tex_arr_friskWalk[playerDir][(int)floor(sprTimer)];

        //Sprite animation timer
        sprTimer += (.03 * dt);

        while (sprTimer >= 4) {
            sprTimer -= 4;
        }

        int i;

        for (i = 0; i < rooms[room].num_exit; ++i) {
            struct exit next = rooms[room].exits[i];
            if (player_pos.x >= next.collision[0].x &&
                player_pos.y >= next.collision[0].y &&
                player_pos.x <= next.collision[1].x &&
                player_pos.y <= next.collision[1].y) {
                    room = next.room_id;
                    player_pos = next.entrance;
                    break;
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
    hidExit();
    aptExit();
    srvExit();

    return 0;
}
