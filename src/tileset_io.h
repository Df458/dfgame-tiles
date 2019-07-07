#ifndef DF_TILES_TILESET_IO
#define DF_TILES_TILESET_IO
#include "tileset.h"
#include <libxml/xmlwriter.h>

/** Callback function for getting a tile's mask from XML */
delegate(uint8, mask_fn, xmlNodePtr);

// Loads a tileset from path
tileset load_tileset(const char* path);

// Saves a tileset to path. texture_file should point to the relative location for the set's texture (this image file does not need to be present, and will not be accessed until the map is loaded)
void save_tileset(const char* path, tileset set);

// Read a tileset's data from xml. The tileset can contain properties or a file reference.
void xml_read_tileset(xmlNodePtr root, tileset* set, const char* path, bool partial);

/** @brief Read a tileset's data from XML. This function is specifically for Tiled (.tsx) files
 *
 * @param root The root XML node
 * @param set The tileset to populate
 * @param path The filepath to use for resolving relative paths
 * @param fn Callback for setting the tile mask. Set to NULL to leave it empty.
 */
void xml_read_tiled_tileset(xmlNodePtr root, tileset* set, const char* path, mask_fn fn);

// Write a tileset's data to xml.
// If force_write_properties is false and asset_path is non-NULL,
// a reference to the tileset's file will be written.
void xml_write_tileset(tileset set, xmlTextWriter* writer, const char* path, bool force_write_properties);

#endif
