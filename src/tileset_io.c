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
    check_return(root, "Tileset file %s is invalid", set, path);

    char* file = NULL;
    check_return(xml_property_read(root, "file", &file), "Tileset at path %s does not specify a texture", set, path);
    char* full_path = combine_paths(get_folder(path), file, true);
    set.tex = load_texture_gl(full_path);
    sfree(full_path);

    xml_property_read(root, "width", &set.width);
    xml_property_read(root, "height", &set.height);
    xml_property_read(root, "set_x", &set.offset.x);
    set.offset.x /= (float)set.tex.width;
    xml_property_read(root, "set_y", &set.offset.y);
    set.offset.y /= (float)set.tex.height;
    xml_property_read(root, "tile_offset_x", &set.tile_box.position.x);
    set.tile_box.position.x /= (float)set.tex.width;
    xml_property_read(root, "tile_offset_y", &set.tile_box.position.y);
    set.tile_box.position.y /= (float)set.tex.height;
    xml_property_read(root, "tile_width", &set.tile_box.dimensions.x);
    set.tile_box.dimensions.x /= (float)set.tex.width;
    xml_property_read(root, "tile_height", &set.tile_box.dimensions.y);
    set.tile_box.dimensions.y /= (float)set.tex.height;

    if(check_error(set.offset.x - set.tile_box.position.x + (set.tile_box.dimensions.x + set.tile_box.position.x) * set.width <= 1 || set.offset.y - set.tile_box.position.y + (set.tile_box.dimensions.y + set.tile_box.position.y) * set.height <= 1, "Dimensions of tileset %s are larger than the texture it uses", path)) {
        glDeleteTextures(1, &set.tex.handle);
        return tileset_empty;
    }

    int16 x = -1;
    int16 y = -1;
    uint8 mask = -1;
    for(xmlNodePtr node = xml_match_name(root->children, "tile"); node; node = xml_match_name(node->next, "tile")) {
        if(xml_property_read(node, "x", &x) && xml_property_read(node, "y", &y) && xml_property_read(node, "mask", &mask)) {
            tileset_set_mask(&set, y * set.width + x, mask);
        }
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
    xmlTextWriterStartElement(writer, (xmlChar*)"tileset");

    xml_property_write(writer, "width", set.width);
    xml_property_write(writer, "height", set.height);
    xml_property_write(writer, "set_x", (uint16)(set.offset.x * set.tex.width));
    xml_property_write(writer, "set_y", (uint16)(set.offset.y * set.tex.height));
    xml_property_write(writer, "tile_offset_x", (uint16)(set.tile_box.position.x * set.tex.width));
    xml_property_write(writer, "tile_offset_y", (uint16)(set.tile_box.position.y * set.tex.height));
    xml_property_write(writer, "tile_width", (uint16)(set.tile_box.dimensions.x * set.tex.width));
    xml_property_write(writer, "tile_height", (uint16)(set.tile_box.dimensions.y * set.tex.height));

    char* t_path = get_relative_base(path, set.tex.asset_path);
    xml_property_write(writer, "file", set.tex.asset_path + strlen(t_path));
    sfree(t_path);


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

    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}
