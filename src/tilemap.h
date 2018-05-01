#ifndef DF_TILES_TILEMAP
#define DF_TILES_TILEMAP
#include "aabb.h"
#include "mesh.h"
#include "types.h"

#include "tileset.h"

declarep(struct, tilemap)
typedef struct tile {
    uint16 id;
} tile;

// Creates a new empty tilemap
tilemap tilemap_new(uint16 w, uint16 h, tileset set);

// Frees an existing tilemap. If deep is true, frees any associated tileset information.
#define tilemap_free(map, deep) { _tilemap_free(map, deep); map = NULL; }
void _tilemap_free(tilemap map, bool deep);

// Sets the tilemap's tileset
void tilemap_set_tileset(tilemap map, tileset set);

// Sets the tile at [x, y]
void tilemap_set_tile(tilemap map, uint16 x, uint16 y, tile t);

// Returns the tile value at [x, y]. Defaults to 0 and logs a warning if [x,y] is out-of-bounds.
tile tilemap_get_tile(tilemap map, uint16 x, uint16 y);

// Returns the mesh data for map
mesh tilemap_get_mesh(tilemap map);

// Returns the tileset for map
tileset tilemap_get_tileset(tilemap map);

// Returns the width of map
uint16 tilemap_get_width(tilemap map);

// Returns the height of map
uint16 tilemap_get_height(tilemap map);

// Resizes map, leaving the previous contents in the top-left corner
void tilemap_resize(tilemap map, uint16 w, uint16 h);

// Returns the default shader for rendering tilemaps. This will compile the shader if it hasn't been done already.
shader get_tilemap_shader();

// Draws the tilemap with the given shader and transformation matrices
void tilemap_draw(tilemap map, shader s, mat4 model, mat4 view);

#endif
