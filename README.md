# Stellarium

A pretty-much-from-scratch* multibody physics engine.

Things implemented from scratch
- Multibody dynamics formulation based on Kane's Method
- OpenGL-based visualization
- Simple math library
  - Vectors
  - Matrices
  - Rotation representations
    - Quaternion
    - Rotation Matrix
    - Axis-Angle
- Lightweight frame and frame transformation library
- Lightweight test harness
- Integrators
  - Euler
  - rk4

Things not implemented from scratch
- 3D model importing (we use assimp)
- Image importing (we use stb_image)
- Lower-level graphics API calls (we use glfw + glad)
- The C++ standard library ;)

# Building

This project uses the [meson](https://mesonbuild.com/) build system to configure builds, which uses `ninja` by default to do the actual compiling.

To build this project from source you will need to get a copy of `meson` and `ninja`
```shell
sudo apt install meson ninja-build
```

Building and configuring is as simple as
```shell
meson setup build
meson compile -C build
```

To configure and build with the default options you can just run
```shell
make
```

## Building with graphics

Requires `assimp`, `glad`, `glfw`, and `stb`. Meson will search for system installed packages first and will download any dependencies that are needed.

## Building documentation

Requires `doxygen`
```shell
sudo apt install doxygen
```
Building of documentation can be toggled with the `build_docs` option, defined in `meson.options`

## Building tests

Building of test can be toggled with the `build_tests` option, defined in `meson.options`

# Running tests

Assuming tests are built, you can run
```shell
make test
```
To run the test full suite. This is just a wrapper for `meson test` which can be used directly for more control
