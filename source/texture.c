#include "texture.h"

#include <sfil.h>
sf2d_texture* loadTexture(char const *texName) {
	char path[80] = "tex/";
	strcat(path, texName);
	strcat(path, ".png");
	return sfil_load_PNG_file(path, SF2D_PLACE_RAM);
}
