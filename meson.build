project('dfgame-tiles', 'c',
        version : '0.1.0',
        license : 'MIT',
        default_options : ['c_std=gnu11'])

compiler = meson.get_compiler('c')

pkgconfig = import('pkgconfig')

m = compiler.find_library('m', required : false)
tidy = find_program('clang-tidy', required: false)
gl = dependency('gl')
xml = dependency('libxml-2.0')

dfgame      = subproject('dfgame')
core        = dfgame.get_variable('core')
math        = dfgame.get_variable('math')
gameplay    = dfgame.get_variable('gameplay')
graphics    = dfgame.get_variable('graphics')
resource    = dfgame.get_variable('resource')
application = dfgame.get_variable('application_game')
glsl_gen    = dfgame.get_variable('glsl_gen')

args = []
subdir('src')
subdir('demo')

run_command('ctags', '-R', '.')
