#ifndef DF_TILES_TILESET
#define DF_TILES_TILESET
#include "aabb.h"
#include "texture.h"
#include "types.h"

typedef struct tileset {
    gltex tex;

    vec2 offset;
    aabb_2d tile_box;

    uint16 width;
    uint16 height;

    char* asset_path;

    uint8* tile_mask;
} tileset;

// Returns the uv bounding box for the given tile index
aabb_2d tileset_get_tile(tileset set, uint16 tile);

// Returns the bitmask for he given tile index
uint8 tileset_get_mask(tileset set, uint16 tile);

// Calculates the dimensions of a tile, in pixels
vec2 tileset_get_tile_dims(tileset set);

// Frees up the dnamic contents of a tieset
void tileset_cleanup(tileset* set);

#endif
