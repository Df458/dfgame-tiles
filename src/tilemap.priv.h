#ifndef DF_TILES_TILEMAP_PRIV
#define DF_TILES_TILEMAP_PRIV

typedef struct tilemap {
    mesh m;
    GLuint tile_handle;

    uint16 width;
    uint16 height;

    tileset set;
    tile* tile_data;

    bool is_empty;
    bool tiles_dirty;
    bool mesh_dirty;

    char* asset_path;
}* tilemap;

#endif
