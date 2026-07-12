stell_sources += files(
  'Camera.cpp',
  'GraphicsEngine.cpp',
  'Shader.cpp',
  'TextRenderer.cpp',
  'import/assimp_importer.cpp',
)

stell_compile_defs += '-DSTELL_BUILD_RENDERING'

cmake = import('cmake')

# glfw — try system package first; fall back to cmake subproject
glfw_dep = dependency('glfw3', required: false)
if not glfw_dep.found()
  glfw_opts = cmake.subproject_options()
  glfw_opts.add_cmake_defines({
    'GLFW_BUILD_DOCS':     'OFF',
    'GLFW_BUILD_TESTS':    'OFF',
    'GLFW_BUILD_EXAMPLES': 'OFF',
  })
  glfw_opts.append_compile_args('c', '-w')
  glfw_sub = cmake.subproject('glfw', options: glfw_opts)
  glfw_dep = glfw_sub.dependency('glfw')
endif

# glad — no system package; build via cmake subproject (runs the Python generator)
glad_opts = cmake.subproject_options()
glad_opts.add_cmake_defines({
  'GLAD_INSTALL':                    'OFF',
  'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
})
glad_opts.append_compile_args('c', '-w')
glad_sub = cmake.subproject('glad', options: glad_opts)
glad_dep = glad_sub.dependency('glad')

# stb — header-only; no pkg-config; always pulled from the subproject
stb_proj = subproject('stb', default_options: ['warning_level=0'])
stb_dep  = stb_proj.get_variable('stb_dep')

# assimp — try the system package first, fall back to a cmake subproject
assimp_dep = dependency('assimp', required: false)
if not assimp_dep.found()
  assimp_opts = cmake.subproject_options()
  assimp_opts.add_cmake_defines({
    'ASSIMP_BUILD_TESTS':                    'OFF',
    'ASSIMP_BUILD_ASSIMP_TOOLS':             'OFF',
    'ASSIMP_BUILD_SAMPLES':                  'OFF',
    'ASSIMP_INSTALL':                        'OFF',
    'ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT': 'OFF',
    'ASSIMP_BUILD_OBJ_IMPORTER':             'ON',
    # Disable all exporters — stellarium only loads models, it does not export them.
    # This also avoids a GCC 13 bug in assimp's glTF2 exporter.
    'ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT': 'OFF',
  })
  assimp_opts.append_compile_args('c',   '-w')
  assimp_opts.append_compile_args('cpp', '-w')
  assimp_sub = cmake.subproject('assimp', options: assimp_opts)
  assimp_dep = assimp_sub.dependency('assimp')
endif

# Platform-specific OpenGL / windowing system libraries
cc = meson.get_compiler('cpp')
if host_machine.system() == 'darwin'
  opengl_dep = dependency('appleframeworks', modules: ['OpenGL', 'Cocoa'])
  stell_deps += [glfw_dep, glad_dep, opengl_dep, assimp_dep, stb_dep]
else
  # Linux (X11)
  gl_dep     = dependency('gl')
  x11_dep    = dependency('x11')
  xrandr_dep = dependency('xrandr')
  xi_dep     = dependency('xi')
  thread_dep = dependency('threads')
  dl_dep     = cc.find_library('dl')
  stell_deps += [
    glfw_dep, glad_dep, gl_dep,
    x11_dep, xrandr_dep, xi_dep,
    thread_dep, dl_dep,
    assimp_dep, stb_dep,
  ]
endif
