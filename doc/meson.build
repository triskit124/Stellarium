doxygen = find_program('doxygen', required: true)

# Fetch doxygen-awesome-css for HTML styling
dac_proj     = subproject('doxygen-awesome-css')
awesome_css_dir = dac_proj.get_variable('doxygen_awesome_css_source_dir')

# Generate the Doxyfile — reuse the same @VAR@ names as the CMake template
# so that doc/Doxyfile.in does not need to be modified.
doxyfile_conf = configuration_data()
doxyfile_conf.set('CMAKE_CURRENT_SOURCE_DIR', meson.current_source_dir())
doxyfile_conf.set('CMAKE_SOURCE_DIR',         meson.project_source_root())
doxyfile_conf.set('AWESOME_CSS_DIR',          awesome_css_dir)

doxyfile = configure_file(
  input:         'Doxyfile.in',
  output:        'Doxyfile',
  configuration: doxyfile_conf,
)

# Custom target that runs doxygen and writes output to doc/build/ (source tree,
# matching the original CMake behaviour).
run_target('docs',
  command: [
    'sh', '-c',
    'mkdir -p "$1/build" && doxygen "$2"',
    'sh',                        # $0 — shell's argv[0]
    meson.current_source_dir(),  # $1
    doxyfile,                    # $2
  ],
)
