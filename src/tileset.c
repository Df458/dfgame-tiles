// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tileset.h"

#include "check.h"

const tileset tileset_empty = {{0}};

// Returns the uv bounding box for the given tile index
aabb_2d tileset_get_tile(tileset set, uint16 tile) {
    check_return(tile < set.width * set.height, "Requested tile index %d is out of bounds. (Tileset length is %d)", aabb_2d_zero, tile, set.width * set.height);

    aabb_2d box = { .position = set.offset, .dimensions = set.tile_box.dimensions };

    uint16 tile_x = tile % set.width;
    uint16 tile_y = tile / set.width;
    box.position.x += tile_x * (set.tile_box.dimensions.x + set.tile_box.position.x) - set.tile_box.position.x;
    box.position.y += tile_y * (set.tile_box.dimensions.y + set.tile_box.position.y) - set.tile_box.position.y;
    box.dimensions.y -= 0.1 / (float)set.tex.height;

    return box;
}

// Returns the bitmask for the given tile index
uint8 tileset_get_mask(tileset set, uint16 tile) {
    check_return(tile < set.width * set.height, "Requested tile index %d is out of bounds. (Tileset length is %d)", 0, tile, set.width * set.height);
    if(!set.tile_mask) {
        return 0;
    }

    return set.tile_mask[tile];
}

// Sets the bitmask for the given tile index
void tileset_set_mask(tileset* set, uint16 tile, uint8 mask) {
    check_return(tile < set->width * set->height, "Requested tile index %d is out of bounds. (Tileset length is %d)", );

    if(!set->tile_mask) {
        set->tile_mask = mscalloc(set->width * set->height, uint8);
    }

    set->tile_mask[tile] = mask;
}

// Sets the dimensions (in tiles) of the tileset, and updates the mask accordingly
void tileset_resize(tileset* set, uint16 width, uint16 height) {
    uint16 old_width = set->width;
    uint16 old_height = set->height;
    uint8* old_mask = set->tile_mask;

    set->width = width;
    set->height = height;

    if(old_mask) {
        if(width * height > 0) {
            set->tile_mask = mscalloc(set->width * set->height, uint8);
            for(uint16 i = 0; i < set->height && i < old_height; ++i) {
                for(uint16 j = 0; j < set->width && j < old_width; ++j) {
                    if(old_mask[i * old_width + j] != 0) {
                        set->tile_mask[i * set->width + j] = old_mask[i * old_width + j];
                    }
                }
            }
        } else {
            set->tile_mask = NULL;
        }

        sfree(old_mask);
    }
}

// Calculates the dimensions of a tile, in pixels
vec2 tileset_get_tile_dims(tileset set) {
    return (vec2) { .x = set.tile_box.dimensions.x * set.tex.width, .y = set.tile_box.dimensions.y * set.tex.height };
}

void tileset_cleanup(tileset* set) {
    check_return(set, "Can't cleanup tileset, because it's NULL", );

    gltex_cleanup(&set->tex);
    if(set->asset_path)
        sfree(set->asset_path);
    if(set->tile_mask)
        sfree(set->tile_mask);
}
