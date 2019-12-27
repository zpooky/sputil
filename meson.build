subdir('src')
subdir('external')

sputil_includes = [
  include_directories('src')
]

sputil_lib = static_library('sputil',
              sputil_src,
              include_directories: sputil_includes,
              dependencies: [backward_dep]
              )

sputil_dep = declare_dependency(
              link_with: sputil_lib,
              include_directories: sputil_includes,
              dependencies: [backward_dep]
              )

subdir('test')
