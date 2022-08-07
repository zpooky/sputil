subdir('src')
# subdir('external')
# backward_dep = dependency('backward')
cpp = meson.get_compiler('cpp')
backward_dep = cpp.find_library('backward')
bfd_dep = cpp.find_library('bfd')
dl_dep = cpp.find_library('dl')

sputil_includes = [
  include_directories('src')
]

sputil_lib = static_library('sputil',
              sputil_src,
              include_directories: sputil_includes,
              dependencies: [backward_dep, bfd_dep,dl_dep]
              )

sputil_dep = declare_dependency(
              link_with: sputil_lib,
              include_directories: sputil_includes,
              dependencies: [backward_dep, bfd_dep,dl_dep]
              )

subdir('test')
