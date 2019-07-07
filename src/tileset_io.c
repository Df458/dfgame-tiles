// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tileset_io.h"

#include "core/check.h"
#include "core/stringutil.h"
#include "resource/paths.h"
#include "resource/texture_loader.h"
#include "resource/xmlutil.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

// Loads a tileset from path
tileset load_tileset(const char* path) {
    tileset set = tileset_empty;

    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    check_return(doc, "Failed to load tileset at path %s", set, path);

    xmlNodePtr root = xml_match_name(xmlDocGetRootElement(doc), "tileset");
    check_return(root != NULL, "Tileset file %s is invalid", set, path);

    char* temp_path = NULL;
    if(xml_property_read(root, "path", &temp_path)) {
        error("Tileset file %s redirects to %s, this is not allowed", path, temp_path);
        sfree(temp_path);
        return set;
    }

    const char* ext = get_extension(path);
    if(!strcmp(ext, "tsx")) {
        xml_read_tiled_tileset(root, &set, path, NULL);
    } else {
        xml_read_tileset(root, &set, path, false);
    }
    set.asset_path = nstrdup(path);

    xmlFreeDoc(doc);
    return set;
}

// Saves a tileset to path. texture_file should point to the relative location for the sets texture (this image file does not need to be present, and will not be accessed until the map is loaded)
void save_tileset(const char* path, tileset set) {
    xmlTextWriter* writer = xmlNewTextWriterFilename(path, 0);
    check_return(writer, "Failed to open path %s for writing", , path);

    xmlTextWriterStartDocument(writer, NULL, "ISO-8859-1", NULL);
    xml_write_tileset(set, writer, path, true);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}

// Read a tileset's data from xml. The tileset can contain properties or a file reference.
void xml_read_tileset(xmlNodePtr root, tileset* set, const char* path, bool partial) {
    check_return(root, "Tileset file %s is invalid", , path);

    char* temp_path = NULL;
    if(xml_property_read(root, "path", &temp_path)) {
        char* tileset_file = combine_paths(get_folder(path), temp_path, true);
        *set = load_tileset(tileset_file);
        sfree(tileset_file);
    } else {
        bool tex_changed = false;
        char* file = NULL;
        if(xml_property_read(root, "file", &file)) {
            if(set->tex.asset_path != NULL) {
                // Undo the transformations, so that they can be re-calculated with the new values later
                set->offset.x *= set->tex.width;
                set->offset.y *= set->tex.height;
                set->tile_box.position.x *= set->tex.width;
                set->tile_box.position.y *= set->tex.height;
                set->tile_box.dimensions.x *= set->tex.width;
                set->tile_box.dimensions.y *= set->tex.height;

                gltex_cleanup(&set->tex);
            }

            tex_changed = true;

            char* full_path = combine_paths(get_folder(path), file, true);
            set->tex = load_texture_gl(full_path);
            info("Loading %s", full_path);
            sfree(full_path);
        } else if(!partial) {
            error("Tileset at path %s does not specify a texture", path);
            return;
        }

        vec2 dims = vec2_zero;
        if(xml_property_read(root, "dims", &dims)) {
            tileset_resize(set, (uint16)dims.x, (uint16)dims.y);
        }

        if(xml_property_read(root, "set_position", &set->offset) || tex_changed) {
            set->offset.x /= (float)set->tex.width;
            set->offset.y /= (float)set->tex.height;
        }

        if(xml_property_read(root, "tile_offset", &set->tile_box.position) || tex_changed) {
            set->tile_box.position.x /= (float)set->tex.width;
            set->tile_box.position.y /= (float)set->tex.height;
        }

        if(xml_property_read(root, "tile_dims", &set->tile_box.dimensions) || tex_changed) {
            set->tile_box.dimensions.x /= (float)set->tex.width;
            set->tile_box.dimensions.y /= (float)set->tex.height;
        }

        check_return(set->offset.x - set->tile_box.position.x + (set->tile_box.dimensions.x + set->tile_box.position.x) * set->width <= 1 || set->offset.y - set->tile_box.position.y + (set->tile_box.dimensions.y + set->tile_box.position.y) * set->height <= 1, "Dimensions of tileset %s are larger than the texture it uses", , path);

        int16 x = -1;
        int16 y = -1;
        uint8 mask = -1;
        for(xmlNodePtr node = xml_match_name(root->children, "tile"); node; node = xml_match_name(node->next, "tile")) {
            if(xml_property_read(node, "x", &x) && xml_property_read(node, "y", &y) && xml_property_read(node, "mask", &mask)) {
                tileset_set_mask(set, y * set->width + x, mask);
            }
        }
    }
}

/** @brief Read a tileset's data from XML. This function is specifically for Tiled (.tsx) files
 *
 * @param root The root XML node
 * @param set The tileset to populate
 * @param path The filepath to use for resolving relative paths
 * @param fn Callback for setting the tile mask. Set to NULL to leave it empty.
 */
void xml_read_tiled_tileset(xmlNodePtr root, tileset* set, const char* path, mask_fn fn) {
    check_return(root, "Tileset file %s is invalid", , path);

    char* file = NULL;
    xmlNodePtr image_node = xml_match_name(root->children, "image");
    if (image_node != NULL && xml_property_read(image_node, "source", &file)) {
        char* full_path = combine_paths(get_folder(path), file, true);
        set->tex = load_texture_gl(full_path);
        sfree(full_path);
    } else {
        error("Tileset at path %s does not specify a texture", path);
        return;
    }

    uint16 w, h;
    bool has_tile_dims = xml_property_read(root, "tilewidth", &w) && xml_property_read(root, "tileheight", &h);
    if(has_tile_dims) {
        set->tile_box.dimensions.x = (float)w / (float)set->tex.width;
        set->tile_box.dimensions.y = (float)h / (float)set->tex.height;

        uint16 t_w, t_h;
        if(image_node != NULL && xml_property_read(image_node, "width", &t_w) && xml_property_read(image_node, "height", &t_h)) {
            tileset_resize(set, t_w/w, t_h/h);
        }
    }

    if (fn != NULL) {
        xml_foreach(tile_node, root->children, "tile") {
            uint16 tile;
            if (xml_property_read(tile_node, "id", &tile)) {
                uint8 mask = fn(tile_node);
                if (mask != 0) {
                    tileset_set_mask(set, tile, mask);
                }
            }
        }
    }
}

// Write a tileset's data to xml.
// If force_write_properties is false and asset_path is non-NULL,
// a reference to the tileset's file will be written.
void xml_write_tileset(tileset set, xmlTextWriter* writer, const char* path, bool force_write_properties) {
    xmlTextWriterStartElement(writer, (xmlChar*)"tileset");

    if(!force_write_properties && set.asset_path != NULL) {
        xml_property_write(writer, "path", set.asset_path);
    } else {
        vec2 dims = (vec2){ .x=set.width, .y=set.height };
        xml_property_write(writer, "dims", dims);

        vec2 set_position = (vec2){ .x=(uint16)(set.offset.x * set.tex.width), .y=(uint16)(set.offset.y * set.tex.height) };
        xml_property_write(writer, "set_position", set_position);

        vec2 set_offset = (vec2){ .x=(uint16)(set.tile_box.position.x * set.tex.width), .y=(uint16)(set.tile_box.position.y * set.tex.height) };
        xml_property_write(writer, "tile_offset", set_offset);

        vec2 tile_dims = (vec2){ .x=(uint16)(set.tile_box.dimensions.x * set.tex.width), .y=(uint16)(set.tile_box.dimensions.y * set.tex.height) };
        xml_property_write(writer, "tile_dims", tile_dims);

        if(set.tex.asset_path) {
            char* t_path = get_relative_base(path, set.tex.asset_path);
            xml_property_write(writer, "file", set.tex.asset_path + strlen(t_path));
            sfree(t_path);
        }

        if(set.tile_mask) {
            for(uint16 i = 0; i < set.width * set.height; ++i) {
                if(set.tile_mask[i]) {
                    xmlTextWriterStartElement(writer, (xmlChar*)"tile");
                    xml_property_write(writer, "x", i % set.width);
                    xml_property_write(writer, "y", i / set.width);
                    xml_property_write(writer, "mask", set.tile_mask[i]);
                    xmlTextWriterEndElement(writer);
                }
            }
        }
    }

    xmlTextWriterEndElement(writer);
}
