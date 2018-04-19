#ifndef DF_TILES_TILEMAP_IO
#define DF_TILES_TILEMAP_IO
#include "tilemap.h"

// Loads a tilemap from path, or returns NULL if an error occurs
tilemap load_tilemap(const char* path);

// Saves a tilemap to path. tileset_file should point to the relative location for the map's tileset (this tileset file does not need to be present, and will not be accessed until the map is loaded).
void save_tilemap(const char* path, tilemap map);

#endif
