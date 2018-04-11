shaders = [
    glsl_gen.process(join_paths(meson.current_source_dir(), '../data/shaders/shader_tilemap.gl'))
]

tilesdeps = [ core, graphics, math, resource, xml ]
tilessrc  = [
    'tilemap.c',
    'tileset.c',
    'tileset_io.c',
    'tilemap_io.c'
]
tilesinc  = []
tileslib  = static_library('dfgame_tiles', shaders, tilessrc,
                         include_directories : tilesinc,
                         dependencies : tilesdeps,
                         link_args : args,
                         install : true)

pkgconfig.generate(libraries : tileslib,
                    version : '0.1.0',
                    name : 'dfgame-tiles',
                    filebase : 'dfgame-tiles',
                    extra_cflags : [ '-I${prefix}/include/dfgame/tiles' ],
                    requires : ['libxml-2.0', 'dfgame-core', 'dfgame-graphics', 'dfgame-math', 'dfgame-resource'],
                    libraries : ['-ldfgame_tiles'],
                    description : 'dfgame tiles module, provides tileset/tilemap support')

install_headers(
    ['tilemap.h', 'tileset.h', 'tilemap_io.h', 'tileset_io.h'],
    subdir : 'dfgame/tiles')

tiles = declare_dependency(include_directories : include_directories('.'), link_with : tileslib)
