#include "room.h"

// Construct rooms.
struct room rooms[3] = {
    { // 0
        { // bg
            "torielHouse1", //name
            {40, 0} // pos
        },
        { // collision
            {77,  60}, // pos0
            {305, 188} // pos1
        },
        {0,0}, // max_scroll
        3, // num_exits
    }, { // 1
        {"torielHouse2", {40, 0}},
        {
            {60,  69},
            {320, 190}
        },
        {0,0},
        1,
    }, { // 2
        {"torielHouse3", {0, 72}},
        {
            {0, 130},
            {710, 175}
        },
        {600,0},
        1,
    }
};

// Maybe this should be named exit_init (for now), because the rooms themselves are already constructed, just not the exits.
// I think I'll keep it this way, though, so that when it isn't hard coded...
void room_init() {
    rooms[0].exits = malloc(rooms[0].num_exit * sizeof(struct exit));
    memcpy(rooms[0].exits, (struct exit[3]){
        { // 0 // entrance when starting the game.
            0,
            {190, 160},
            {
                {0, 0},
                {0, 0}
            }
        }, { // 1
            1, // room_id
            {319, 160}, // entrance
            { // collision
                {0, 145}, // pos1
                {78,  195}  // pos2
            }
        }, { // 2
            2,
            {41, 131},
            {
                {281, 145},
                {300, 195}
            }
        }
    }, rooms[0].num_exit * sizeof(struct exit));

    rooms[1].exits = malloc(rooms[1].num_exit * sizeof(struct exit));
    rooms[1].exits[0] = (struct exit){ // More efficent size-wise if not multiple exits.
        0,
        {78, 160},
        {
            {319, 145},
            {400, 195}
        }
    };

    rooms[2].exits = malloc(rooms[2].num_exit * sizeof(struct exit));
    rooms[2].exits[0] = (struct exit){
        0,
        {280, 160},
        {
            {-5, 75},
            {5, 205}
        }
    };
}

struct exit* exit_room(const int roomID, struct position *pos) {
    int i;

    for (i = 0; i < rooms[roomID].num_exit; ++i) {
        struct exit *next = &rooms[roomID].exits[i];
        if (pos->x >= next->collision[0].x &&
            pos->y >= next->collision[0].y &&
            pos->x <= next->collision[1].x &&
            pos->y <= next->collision[1].y) {
                return next;
            }
    }

    return NULL;
}
