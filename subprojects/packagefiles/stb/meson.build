# Meson build override for stb (header-only library collection).
# Exposes the repository root as an include directory so that headers
# like stb_image.h can be included directly.
project('stb', 'c', version: '0.0.1')

stb_dep = declare_dependency(
  include_directories: include_directories('.'),
)
