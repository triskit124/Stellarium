fs = import('fs')

stell_compile_defs += '-DSTELL_BUILD_TESTS'

test_sources = files(
  'test_math/test_vector.cpp',
  'test_math/test_matrix.cpp',
  'test_math/test_matrix33.cpp',
  'test_math/test_quaternion.cpp',
  'test_math/test_transforms.cpp',
  'test_math/test_vector3.cpp',
  'test_sim/test_spatial_algebra.cpp',
  'test_sim/test_pendulum.cpp',
  'test_sim/test_simple_rotation.cpp',
  'test_sim/test_simple_translation.cpp',
)

# Interactive demos. These are built like tests but NOT registered with meson, because they open a
# GL window and run until it is closed -- `meson test` would block on them forever. Run them by
# hand out of the build directory.
demo_sources = []

if get_option('build_rendering')
  demo_sources += files(
    'test_demos/test_double_pendulum.cpp',
    'test_demos/test_slider.cpp',
    'test_demos/test_ball_joint.cpp',
  )
  # Both of these still use the pre-multibody Body/addBody API and need an interactive window;
  # test_window additionally needs assets/backpack/, which is not tracked in the repo.
  # test_sources += files(
  #   'test_graphics/test_window.cpp',
  #   'test_graphics/test_model_rendering.cpp',
  # )
endif

# Add an executable for each test source file, and add a test for it.
foreach src : test_sources
  name = fs.stem(src)
  exe = executable(name,
    src,
    dependencies: stellarium_dep,
  )
  test(name, exe)
endforeach

# Demos are built, but never run as part of the test suite.
foreach src : demo_sources
  executable(fs.stem(src),
    src,
    dependencies: stellarium_dep,
  )
endforeach
