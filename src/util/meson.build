stell_sources += files('TestHarness.cpp')

# Generate Config.h into the build directory.  When compilers search the
# include path, the build-tree copy (here) shadows the stale CMake-generated
# src/util/Config.h in the source tree.
configure_file(
  input:         'Config.h.in',
  output:        'Config.h',      # → <builddir>/src/util/Config.h
  configuration: conf,
)
