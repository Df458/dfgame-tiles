#ifndef DF_TILES_TILESET
#define DF_TILES_TILESET
#include "aabb.h"
#include "texture.h"
#include "types.h"

#define NO_TILE UINT16_MAX

typedef struct tileset {
    gltex tex;

    vec2 offset;
    aabb_2d tile_box;

    uint16 width;
    uint16 height;

    char* asset_path;

    uint8* tile_mask;
} tileset;

const extern tileset tileset_empty;

// Returns the uv bounding box for the given tile index
aabb_2d tileset_get_tile(tileset set, uint16 tile);

// Returns the bitmask for the given tile index
uint8 tileset_get_mask(tileset set, uint16 tile);

// Sets the bitmask for the given tile index
void tileset_set_mask(tileset* set, uint16 tile, uint8 mask);

// Sets the dimensions (in tiles) of the tileset, and updates the mask accordingly
void tileset_resize(tileset* set, uint16 width, uint16 height);

// Calculates the dimensions of a tile, in pixels
vec2 tileset_get_tile_dims(tileset set);

// Frees up the dnamic contents of a tieset
void tileset_cleanup(tileset* set);

#endif
