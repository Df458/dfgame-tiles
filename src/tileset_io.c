// Log category, used to filter logs
#define LOG_CATEGORY "Tiles"

#include "tileset_io.h"

#include "check.h"
#include "paths.h"
#include "stringutil.h"
#include "texture_loader.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

// Loads a tileset from path
tileset load_tileset(const char* path) {
    xmlDocPtr doc = xmlReadFile(path, NULL, 0);
    check_return(doc, "Failed to load tileset at path %s", (tileset){0}, path);

    xmlNodePtr root = xmlDocGetRootElement(doc);
    for(; root; root = root->next)
        if(root->type == XML_ELEMENT_NODE && !xmlStrcmp(root->name, (const xmlChar*)"tileset"))
            break;

    check_return(root, "Tileset file %s is invalid", (tileset){0}, path);

    tileset set = (tileset){0};

    xmlChar* a = NULL;
    if((a = xmlGetProp(root, (const xmlChar*)"width"))) {
        set.width = atoi((char*)a);
        sfree(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"height"))) {
        set.height = atoi((char*)a);
        sfree(a);
    }

    if((a = xmlGetProp(root, (const xmlChar*)"file"))) {
        char* full_path = combine_paths(get_folder(path), (char*)a, true);
        set.tex = load_texture_gl(full_path);
        sfree(full_path);
    } else {
        warn("Tileset at path %s does not specify a texture", path);

        return (tileset){0};
    }

    if((a = xmlGetProp(root, (const xmlChar*)"set_x"))) {
        set.offset.x = (float)atoi((char*)a) / (float)set.tex.width;
        sfree(a);
    } else {
        set.offset.x = 0;
    }
    if((a = xmlGetProp(root, (const xmlChar*)"set_y"))) {
        set.offset.y = (float)atoi((char*)a) / (float)set.tex.height;
        sfree(a);
    } else {
        set.offset.y = 0;
    }
    if((a = xmlGetProp(root, (const xmlChar*)"tile_offset_x"))) {
        set.tile_box.position.x = (float)atoi((char*)a) / (float)set.tex.width;
        sfree(a);
    } else {
        set.tile_box.position.x = 0;
    }
    if((a = xmlGetProp(root, (const xmlChar*)"tile_offset_y"))) {
        set.tile_box.position.y = (float)atoi((char*)a) / (float)set.tex.height;
        sfree(a);
    } else {
        set.tile_box.position.y = 0;
    }
    if((a = xmlGetProp(root, (const xmlChar*)"tile_width"))) {
        set.tile_box.dimensions.x = (float)atoi((char*)a) / (float)set.tex.width;
        sfree(a);
    }
    if((a = xmlGetProp(root, (const xmlChar*)"tile_height"))) {
        set.tile_box.dimensions.y = (float)atoi((char*)a) / (float)set.tex.height;
        sfree(a);
    }

    if(check_error(set.offset.x - set.tile_box.position.x + (set.tile_box.dimensions.x + set.tile_box.position.x) * set.width <= 1 || set.offset.y - set.tile_box.position.y + (set.tile_box.dimensions.y + set.tile_box.position.y) * set.height <= 1, "Dimensions of tileset %s are larger than the texture it uses", path)) {
        glDeleteTextures(1, &set.tex.handle);
        return (tileset){0};
    }

    set.asset_path = nstrdup(path);

    xmlFreeDoc(doc);

    return set;
}

// Saves a tileset to path. texture_file should point to the relative location for the set's texture (this image file does not need to be present, and will not be accessed until the map is loaded)
void save_tileset(const char* path, tileset set) {
    xmlTextWriter* writer = xmlNewTextWriterFilename(path, 0);
    check_return(writer, "Failed to open path %s for writing", , path);

    xmlTextWriterStartDocument(writer, NULL, "ISO-8859-1", NULL);
    xmlTextWriterStartElement(writer, (xmlChar*)"tileset");

    char* a = saprintf("%d", set.width);
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"width", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", set.height);
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"height", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.offset.x * set.tex.width));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"set_x", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.offset.y * set.tex.height));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"set_y", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.tile_box.position.x * set.tex.width));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"tile_offset_x", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.tile_box.position.y * set.tex.height));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"tile_offset_y", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.tile_box.dimensions.x * set.tex.width));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"tile_width", (xmlChar*)a);
    sfree(a);
    a = saprintf("%d", (int)(set.tile_box.dimensions.y * set.tex.height));
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"tile_height", (xmlChar*)a);
    sfree(a);

    char* t_path = get_relative_base(path, set.tex.asset_path);
    xmlTextWriterWriteAttribute(writer, (xmlChar*)"file", (xmlChar*)(set.tex.asset_path + strlen(t_path)));
    sfree(t_path);

    xmlTextWriterEndElement(writer);
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}
