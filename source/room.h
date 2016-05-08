#pragma once
// PARANOID
#ifndef UT_TD_ROOM
#define UT_TD_ROOM

#include <stdlib.h>
#include <string.h>

#include "texture.h"
#include "common.h"

struct exit;

struct room {
    struct texture bg;
    position collision[2];
    position scroll_max;
    unsigned int num_exit;
    struct exit *exits;
};

struct exit {
    int room_id;
    position entrance;
    position collision[2]; // TODO: Consider replacing array with seperate positions min and max?
};

extern struct room rooms[3];

void room_init(void);

int exit_room(const int roomID, struct position *pos);

#endif /* end of include guard: UT_TD_ROOM */
