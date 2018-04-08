# DFGame Tiles Module
DFGame is a collection of libraries designed to remove much of the boilerplate
involved in "from scratch" game development. This module provides tileset/tilemap
support for projects using DFGame.

## Features
Currently, this module is in an MVP state, so the featureset is limited to:
- Single-layer, single-tileset maps
- Rectangular tiles only (Nothing fancy like isometric or hex tiles)
- Saving/Loading tilesets/tilemaps
- Rendering tilemaps
- Setting/Querying tiles in tilemaps

## Building This Library
To build this library, you'll need the following tools:
- meson
- ninja
- pkg-config (optional)
as well as all DFGame dependencies.

You can build and install this library using the following steps.
```bash
# Create build directory and generate project files
mkdir build && cd build
meson ..

# Compile and install
ninja
ninja install
```
