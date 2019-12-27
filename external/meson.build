cpp = meson.get_compiler('cpp')
dl_dep = cpp.find_library('dl')
bfd_dep = cpp.find_library('bfd')

backward_include = include_directories('backward-cpp')

backward_dep = declare_dependency(
                include_directories: backward_include,
                dependencies: [dl_dep, bfd_dep]
              )

