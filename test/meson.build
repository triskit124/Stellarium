fs = import('fs')

stell_compile_defs += '-DSTELL_BUILD_TESTS'

test_sources = files(
  'test_math/test_vector.cpp',
  'test_math/test_matrix.cpp',
  'test_math/test_matrix33.cpp',
  'test_math/test_quaternion.cpp',
  'test_math/test_transforms.cpp',
  'test_math/test_vector3.cpp',
  # 'test_sim/test_simple_rotation.cpp',
  # 'test_sim/test_simple_translation.cpp',
)

if get_option('build_rendering')
  test_sources += files(
    # 'test_graphics/test_window.cpp',
    # 'test_graphics/test_model_rendering.cpp'
    'test_demos/test_double_pendulum.cpp',
  )
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
