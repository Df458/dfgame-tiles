#ifndef DF_TILES_TILESET_IO
#define DF_TILES_TILESET_IO
#include "tileset.h"

// Loads a tileset from path
tileset load_tileset(const char* path);

// Saves a tileset to path. texture_file should point to the relative location for the set's texture (this image file does not need to be present, and will not be accessed until the map is loaded)
void save_tileset(const char* path, tileset set, const char* texture_file);

#endif
