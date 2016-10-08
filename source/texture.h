#pragma once
// Header guard for paranoia.
#ifndef UT_TD_TEXTURE
#define UT_TD_TEXTURE

#include <sf2d.h>
#include "common.h"

struct texture {
    const char *name;
    position pos;
    sf2d_texture *tex;
};

sf2d_texture* loadTexture(const char *texName);

void fillTexture(struct texture *tex);

#endif /* end of include guard: UT_TD_TEXTURE */
