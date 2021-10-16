project('dibibase', 'cpp',
  version : '0.1.0',
  license: 'Apache-2.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

cppc = meson.get_compiler('cpp')

dep_antlr4 = dependency('antlr4-runtime', required: false)
if not dep_antlr4.found()
  # find the lib without pkg-config
  dep_antlr4 = cppc.find_library('antlr4-runtime')
endif

gtest_proj = subproject('gtest')

inc = include_directories('include')

subdir('include')
subdir('lib')
subdir('src')
subdir('tests')

# Make this library usable as a Meson subproject.
dibibase_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with : dibibaselib)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'libdibibase',
  filebase : 'dibibase',
  description : 'A Distributed Wide-Column NoSQL Database.',
  subdirs : 'dibibase',
  libraries : dibibaselib,
  version : '0.1.0',
)
