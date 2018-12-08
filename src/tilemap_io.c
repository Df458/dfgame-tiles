// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tilemap_io.h"
#include "tileset_io.h"

#include "core/check.h"
#include "core/log/log.h"
#include "core/stringutil.h"
#include "resource/paths.h"

#include <stdio.h>

#include "tilemap.priv.h"

// Loads a tilemap from path, or returns NULL if an error occurs
tilemap load_tilemap(const char* path) {
    uint16 w, h;
    tile t;
    ssize_t plen;
    char* tileset_path = NULL;

    FILE* infile = fopen(path, "re");

    check_return(infile, "Can't open tilemap file at %s", NULL, path);
    
    size_t elements = fread(&w, sizeof(w), 1, infile) + fread(&h, sizeof(h), 1, infile) + fread(&plen, sizeof(plen), 1, infile);
    if(check_error(elements == 3, "Can't load tilemap %s: Invalid Header", path)) {
        fclose(infile);
        return NULL;
    }

    if(plen > 0) {
        tileset_path = mscalloc(plen + 1, char);
        size_t read = fread(tileset_path, sizeof(char), plen, infile);
        if(check_error(read == plen, "Can't load tilemap %s: Size mismatch in tileset path (%d != %d)", path, read, plen)) {
            fclose(infile);
            return NULL;
        }
        tileset_path = combine_paths(get_folder(path), tileset_path, true);
    }

    tilemap map = tilemap_new(w, h);
    map->asset_path = nstrdup(path);
    if(tileset_path != NULL) {
        tilemap_set_tileset(map, load_tileset(tileset_path));

        for(int i = 0; i < h; ++i) {
            for(int j = 0; j < w; ++j) {
                if(check_warn(fread(&t, sizeof(tile), 1, infile) == 1, "Unexpected end of file while reading tile data. Tilemap may be incomplete.")) {
                    break;
                }
                tilemap_set_tile(map, j, i, t.id);
            }
        }
    }

    fclose(infile);

    return map;
}

// Saves a tilemap to path. tileset_file should point to the relative location for the map's tileset (this tileset file does not need to be present, and will not be accessed until the map is loaded).
void save_tilemap(const char* path, tilemap map) {
    FILE* outfile = fopen(path, "we");

    check_return(outfile != NULL, "Failed to save tilemap: Can't open file at %s", , path);

    fwrite(&(map->width), sizeof(map->width), 1, outfile);
    fwrite(&(map->height), sizeof(map->height), 1, outfile);

    ssize_t len = 0;
    if(map->set.asset_path != NULL) {
        char* t_path = get_relative_base(path, map->set.asset_path);
        len = strlen(map->set.asset_path) - strlen(t_path);
        fwrite(&(len), sizeof(len), 1, outfile);

        fwrite(map->set.asset_path + strlen(t_path), sizeof(char), len, outfile);
        sfree(t_path);

        for(int i = 0; i < map->width * map->height; ++i) {
            fwrite(map->tile_data, sizeof(tile), map->width * map->height, outfile);
        }
    } else {
        // If there's no tileset, there's nothing to write.
        fwrite(&(len), sizeof(len), 1, outfile);
    }

    fclose(outfile);
}
