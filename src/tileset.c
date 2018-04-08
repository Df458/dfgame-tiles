// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tileset.h"

#include "check.h"

// Returns the uv bounding box for the given tile index
aabb_2d tileset_get_tile(tileset set, uint16 tile) {
    check_return(tile < set.width * set.height, "Requested tile index %d is out of bounds. (Tileset length is %d)", (aabb_2d){0}, tile, set.width * set.height);

    aabb_2d box = { .position = set.offset, .dimensions = set.tile_box.dimensions };

    box.position.x += (tile % set.width) * (set.tile_box.dimensions.x + set.tile_box.position.x) - set.tile_box.position.x;
    box.position.y += (tile / set.height) * (set.tile_box.dimensions.y + set.tile_box.position.y) - set.tile_box.position.y;

    return box;
}

// Calculates the dimensions of a tile, in pixels
vec2 tileset_get_tile_dims(tileset set) {
    return (vec2) { .x = set.tile_box.dimensions.x * set.tex.width, .y = set.tile_box.dimensions.y * set.tex.height };
}
