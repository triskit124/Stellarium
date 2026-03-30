######################################################################################
#   ███████ ████████ ███████ ██      ██       █████  ██████  ██ ██    ██ ███    ███
#   ██         ██    ██      ██      ██      ██   ██ ██   ██ ██ ██    ██ ████  ████
#   ███████    ██    █████   ██      ██      ███████ ██████  ██ ██    ██ ██ ████ ██
#        ██    ██    ██      ██      ██      ██   ██ ██   ██ ██ ██    ██ ██  ██  ██
#   ███████    ██    ███████ ███████ ███████ ██   ██ ██   ██ ██  ██████  ██      ██
######################################################################################

project('stellarium', 'cpp',
  version: '0.2.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=debugoptimized',
  ],
)

# Options
build_rendering = get_option('build_rendering')
build_tests     = get_option('build_tests')
build_docs      = get_option('build_docs')

# Confiuguration data
conf = configuration_data()
conf.set('PROJECT_ROOT', meson.project_source_root())

stell_includes = include_directories(
  'src/core', 
  'src/math', 
  'src/render', 
  'src/util'
)
stell_sources = []
stell_deps    = []
stell_compile_defs = []

subdir('src')

stellarium = library('stellarium',
  stell_sources,
  include_directories: stell_includes,
  dependencies:        stell_deps,
  cpp_args:            stell_compile_defs,
  install:             true,
)

stellarium_dep = declare_dependency(
  link_with:           stellarium,
  include_directories: stell_includes,
  compile_args:        stell_compile_defs,
  dependencies:        stell_deps,
)

if build_tests
  subdir('test')
endif

if build_docs
  subdir('doc')
endif
