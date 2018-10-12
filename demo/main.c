#include "camera.h"
#include "tilemap.h"
#include "tilemap_io.h"
#include "tileset_io.h"

#include "font_loader.h"
#include "input.h"
#include "log/log.h"
#include "mainloop.h"
#include "matrix.h"
#include "shader_init.h"
#include "paths.h"
#include "text.h"
#include "transform.h"
#include "vector.h"
#include "window.h"

#include <time.h>
#include <stdlib.h>

static window win = NULL;

static tilemap map = NULL;
static camera c_main = NULL;

static shader s_text;

static text info_text;

#define TILEMAP_NAME "tilemap.dat"
#define TILESET_NAME "tileset.xml"
#define MAP_DIM 20

const char* const info_str =
"Controls\n"
"Space: Randomize Tiles\n"
"S: Save Map\n"
"L: Load Map";

void randomize(action_id id, void* user) {
    tileset set = tilemap_get_tileset(map);
    uint16 w = tilemap_get_width(map);
    uint16 h = tilemap_get_height(map);

    // Fill every map space with random tile indices, calculated from the
    // tileset dimensions.
    for(int i = 0; i < w * h; ++i) {
        tilemap_set_tile(map, i % w, i / w, rand() % (set.width * set.height));
    }
}

void save(action_id id, void* user) {
    char* p = assets_path(TILEMAP_NAME, NULL);
    save_tilemap(p, map);
    sfree(p);
}

void load(action_id id, void* user) {
    tilemap_free(map, true); // Destroy the old map, to avoid leaking memory

    char* p = assets_path(TILEMAP_NAME, NULL);
    map = load_tilemap(p);
    sfree(p);

    // Sanity check: If the map failed to load, make a blank map to fill its place.
    // We could also keep the old map, but this method provides some sort of feedback.
    if(!map) {
        map = tilemap_new(MAP_DIM, MAP_DIM);
        tilemap_set_tileset(map, load_tileset(assets_path(TILESET_NAME, NULL)));
    }
}

bool loop_fn(mainloop l, float dt) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate offset to center the map, and draw
    vec2 offset = vec2_mul(tileset_get_tile_dims(tilemap_get_tileset(map)), -MAP_DIM * 0.5f);
    tilemap_draw(map, get_tilemap_shader(), mat4_translate(mat4_ident, offset), camera_get_vp(c_main));

    // Draw text
    glUseProgram(s_text.id);
    offset = (vec2){ .x = -634, .y=354 };
    text_draw(info_text, s_text, mat4_mul(camera_get_vp(c_main), mat4_translate(mat4_ident, offset)));

    window_redraw(win);
    return !window_get_should_close(win);
}

int main(int argc, char** argv) {
    // Init window/paths/shaders
    win = window_new(1280, 720, false, "Tiles Demo");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    init_base_resource_path(NULL);
    shaders_init();

    s_text = shader_basic_tex_get();

    srand(time(NULL));

    // Bind inpouts for this demo
    input_add_key_action(K_SPACE, as_event(action_event, randomize, NULL));
    input_add_key_action(K_S, as_event(action_event, save, NULL));
    input_add_key_action(K_L, as_event(action_event, load, NULL));

    // Create help text
    char* p = assets_path("OpenSans-Regular.ttf", NULL);
    info_text = text_new(load_font(p, 16), info_str);
    text_set_align(info_text, TEXT_ALIGN_BOTTOM_LEFT);
    sfree(p);

    // Create camera and map
    c_main = window_create_2d_camera(win);
    map = tilemap_new(MAP_DIM, MAP_DIM);
    tilemap_set_tileset(map, load_tileset(assets_path(TILESET_NAME, NULL)));

    mainloop_create_run(loop_fn);

    tilemap_free(map, true);
    resource_path_free();
    window_free(win);

    return 0;
}
