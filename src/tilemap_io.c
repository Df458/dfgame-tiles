// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tilemap_io.h"

#include "core/check.h"
#include "core/log/log.h"
#include "core/stringutil.h"
#include "resource/paths.h"
#include "tileset_io.h"

#include <stdio.h>

typedef struct tilemap {
    mesh m;
    GLuint tile_handle;

    uint16 width;
    uint16 height;

    tileset set;
    tile* tile_data;

    char* asset_path;
}* tilemap;

// Loads a tilemap from path, or returns NULL if an error occurs
tilemap load_tilemap(const char* path) {
    uint16 w, h;
    tile t;
    ssize_t plen;
    char* tileset_path;

    FILE* infile = fopen(path, "r");

    check_return(infile, "Can't open tilemap file at %s", NULL, path);
    
    size_t elements = fread(&w, sizeof(w), 1, infile) + fread(&h, sizeof(h), 1, infile) + fread(&plen, sizeof(plen), 1, infile);
    if(check_error(elements == 3, "Can't load tilemap %s: Invalid Header", path)) {
        fclose(infile);
        return NULL;
    }

    tileset_path = mscalloc(plen + 1, char);
    size_t read = fread(tileset_path, sizeof(char), plen, infile);
    if(check_error(read == plen, "Can't load tilemap %s: Size mismatch in tileset path (%d != %d)", path, read, plen)) {
        fclose(infile);
        return NULL;
    }
    tileset_path = combine_paths(get_folder(path), tileset_path, true);

    tilemap map = tilemap_new(w, h, load_tileset(tileset_path));
    map->asset_path = nstrdup(path);

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            if(check_warn(fread(&t, sizeof(tile), 1, infile) == 1, "Unexpected end of file while reading tile data. Tilemap may be incomplete.")) {
                break;
            }
            tilemap_set_tile_fast(map, j, i, t.id);
        }
    }

    tilemap_update_tiles(map);

    fclose(infile);

    return map;
}

// Saves a tilemap to path. tileset_file should point to the relative location for the map's tileset (this tileset file does not need to be present, and will not be accessed until the map is loaded).
void save_tilemap(const char* path, tilemap map) {
    FILE* outfile = fopen(path, "w");

    fwrite(&(map->width), sizeof(map->width), 1, outfile);
    fwrite(&(map->height), sizeof(map->height), 1, outfile);

    char* t_path = get_relative_base(path, map->set.asset_path);
    ssize_t len = strlen(map->set.asset_path) - strlen(t_path);
    fwrite(&(len), sizeof(len), 1, outfile);

    fwrite(map->set.asset_path + strlen(t_path), sizeof(char), len, outfile);
    sfree(t_path);

    for(int i = 0; i < map->width * map->height; ++i) {
        fwrite(map->tile_data, sizeof(tile), map->width * map->height, outfile);
    }

    fclose(outfile);
}
