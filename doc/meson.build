
# Configure the Doxyfile
doxyfile_conf = configuration_data()
doxyfile_conf.set('PROJECT_ROOT', meson.project_source_root())
doxyfile_conf.set('DOXYGEN_OUTPUT_DIR', meson.current_build_dir())

doxyfile = configure_file(
  input:         'Doxyfile.in',
  output:        'Doxyfile',
  configuration: doxyfile_conf,
)

# Run Doxygen to generate the documentation
doxygen = find_program('doxygen', required: true)
custom_target('docs',
    input: doxyfile,
    output: 'html',
    command: [doxygen, doxyfile],
    build_by_default: true,
)