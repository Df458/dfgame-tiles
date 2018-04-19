demo_deps = [
    application,
    gameplay
]
demo_src  = [ 
    'main.c'
]

demo_target = executable('demo_tiles',
        demo_src,
        include_directories : include_directories('../src'),
        dependencies : [tilesdeps, demo_deps],
        link_with : tileslib,
        link_args : args,
        install : false)
run_command('ctags', '-R', '.')
run_command('ln', '-rs', 'assets', '../build/demo')
