// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tilemap.h"

#include "shader_tilemap.h"

#include "check.h"
#include "container/array.h"
#include "matrix.h"

static shader shader_tilemap = {0};

typedef struct tilemap {
    mesh m;
    GLuint tile_handle;

    uint16 width;
    uint16 height;

    tileset set;
    tile* tile_data;

    char* asset_path;
}* tilemap;

// Rebuilds the tile data buffer for map
void tilemap_update_tiles(tilemap map) {
    aabb_2d tiles[map->width * map->height];
    for(int i = 0; i < map->height; ++i) {
        for(int j = 0; j < map->width; ++j) {
            tiles[i * map->width + j] = tileset_get_tile(map->set, map->tile_data[i * map->width + j].id);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, map->tile_handle);
    glBufferData(GL_ARRAY_BUFFER, map->width * map->height * sizeof(aabb_2d), tiles, GL_DYNAMIC_DRAW);
}

// Rebuilds the mesh data for map
void tilemap_rebuild_mesh(tilemap map) {
    if(map->m)
        mesh_free(map->m);

    vt_p mesh_data[map->width * map->height];
    for(int i = 0; i < map->height; ++i) {
        for(int j = 0; j < map->width; ++j) {
            mesh_data[i * map->width + j].position = (vec3){.x = j, .y = i, .z = 0};
        }
    }
    map->m = mesh_new(map->width * map->height, mesh_data, NULL);

    tilemap_update_tiles(map);
}

// Creates a new empty tilemap
tilemap tilemap_new(uint16 w, uint16 h, tileset set) {
    check_return(w * h != 0, "Trying to create a tilemap with the invalid dimensions [%dx%d]", NULL, w, h);

    tilemap map = mscalloc(1, struct tilemap);

    map->width = w;
    map->height = h;
    map->tile_data = mscalloc(w * h, tile);
    map->set = set;
    map->asset_path = NULL;

    glGenBuffers(1, &map->tile_handle);
    tilemap_rebuild_mesh(map);

    return map;
}

// Frees an existing tilemap. If deep is true, frees any associated tileset information.
void _tilemap_free(tilemap map, bool deep) {
    sfree(map->tile_data);

    if(deep)
        tileset_cleanup(&map->set);

    glDeleteBuffers(1, &map->tile_handle);

    mesh_free(map->m);
    if(map->asset_path)
        sfree(map->asset_path);
    sfree(map);
}

// Sets the tilemap's tileset
void tilemap_set_tileset(tilemap map, tileset set) {
    // Warn the user if there's the possibility of out-of-range tiles
    check_warn(map->set.width * map->set.height <= set.width * set.height, "Setting a tileset with smaller dimensions than before, some tiles may be invalid");

    map->set = set;
    tilemap_update_tiles(map);
}

// Sets the tile at [x, y]
void tilemap_set_tile(tilemap map, uint16 x, uint16 y, tile t) {
    check_return(x < map->width && y < map->height, "Can't set out-of-bounds tile at [%d, %d] from a %dx%d map", , x, y, map->width, map->height);

    map->tile_data[y * map->width + x] = t;

    tilemap_update_tiles(map);
}

// Returns the tile value at [x, y]. Defaults to 0 and logs a warning if [x,y] is out-of-bounds.
tile tilemap_get_tile(tilemap map, uint16 x, uint16 y) {
    check_return(x < map->width && y < map->height, "Can't get out-of-bounds tile at [%d, %d] from a %dx%d map", (tile){0}, x, y, map->width, map->height);
    return map->tile_data[y * map->width + x];
}

// Returns the mesh data for map
mesh tilemap_get_mesh(tilemap map) {
    return map->m;
}

// Returns the tileset for map
tileset tilemap_get_tileset(tilemap map) {
    return map->set;
}

// Returns the width of map
uint16 tilemap_get_width(tilemap map) {
    return map->width;
}

// Returns the height of map
uint16 tilemap_get_height(tilemap map) {
    return map->height;
}

// Resizes map, leaving the previous contents in the top-left corner
void tilemap_resize(tilemap map, uint16 w, uint16 h) {
    check_return(w * h != 0, "Trying to resize a map to invalid dimensions [%dx%d]", , w, h);
    tile* new_data = mscalloc(w * h, tile);

    // Copy the data from the old array to the new one. Note that
    // bounds-checking must be performed on *both* buffers, since either one
    // could be smaller.
    for(int i = 0; i < map->height && i < h; ++i) {
        for(int j = 0; j < map->width && j < w; ++j) {
            new_data[i * w + j] = map->tile_data[i * map->width + j];
        }
    }

    sfree(map->tile_data);
    map->tile_data = new_data;
    map->width = w;
    map->height = h;
    tilemap_rebuild_mesh(map);
}

// Returns the default shader for rendering tilemaps. This will compile the shader if it hasn't been done already.
shader get_tilemap_shader() {
    if(shader_tilemap.id == 0)
        shader_tilemap = compile_shader_tilemap();

    return shader_tilemap;
}

// Draws the tilemap with the given shader and transformation matrices
void tilemap_draw(tilemap map, shader s, mat4 model, mat4 view) {
    glUseProgram(s.id);
    shader_bind_uniform_name(s, "u_transform", model);
    shader_bind_uniform_name(s, "u_view", view);
    shader_bind_uniform_name(s, "u_dims", tileset_get_tile_dims(map->set));
    shader_bind_uniform_texture_name(s, "u_texture", map->set.tex, GL_TEXTURE0);

    glBindBuffer(GL_ARRAY_BUFFER, map->tile_handle);
    GLuint attrib = glGetAttribLocation(s.id, "i_uv");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    mesh_render(s, map->m, GL_POINTS, "i_pos", VT_POSITION);
}
