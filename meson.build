subdir('src')
# subdir('external')

sputil_deps = []
sputil_cpp_args = []

# backward_dep = dependency('backward')
cpp = meson.get_compiler('cpp')
backward_dep = cpp.find_library('backward', required: false)
if backward_dep.found()
  bfd_dep = cpp.find_library('bfd')
  dl_dep = cpp.find_library('dl')
  sputil_deps += [backward_dep, bfd_dep, dl_dep]
  sputil_cpp_args += '-DUSE_BACKWARDS'
endif

sputil_includes = [
  include_directories('src')
]

sputil_lib = static_library('sputil',
              sputil_src,
              include_directories: sputil_includes,
              dependencies: sputil_deps,
              cpp_args: sputil_cpp_args
              )

sputil_dep = declare_dependency(
              link_with: sputil_lib,
              include_directories: sputil_includes,
              dependencies: sputil_deps
              )

subdir('test')
