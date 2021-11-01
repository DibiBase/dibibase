project('dibibase', 'cpp',
  version : '0.1.0',
  license: 'Apache-2.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# add_project_arguments('-glldb', language : 'cpp')

if meson.version().version_compare('>=0.56.0')
  project_source_root = meson.project_source_root()
else
  project_source_root = meson.source_root()
endif

cppc = meson.get_compiler('cpp')

inc = include_directories('include')

## ANTLR Dependencies
dep_antlr4 = dependency('antlr4-runtime', required: false)
if not dep_antlr4.found()
  # find the lib without pkg-config
  dep_antlr4 = cppc.find_library('antlr4-runtime')
endif

## Grammar Dependencies
antlr4 = find_program('antlr4')

src_grammar = custom_target('gen-grammar',
  input : [
    project_source_root + '/grammars/ebnf/CqlLexer.g4',
    project_source_root + '/grammars/ebnf/CqlParser.g4',
  ],
  output : [
    'CqlLexer.h',
    'CqlLexer.cpp',
    'CqlParser.h',
    'CqlParser.cpp',
    'CqlParserVisitor.h',
    'CqlParserBaseVisitor.h',
  ],
  command : [
    antlr4,
    '-Dlanguage=Cpp',
    '-DexportMacro=DIBIBASE_PUBLIC',
    '-visitor',
    '-o', '@OUTDIR@',
    '-package', 'dibibase::lang::cql3',
    '-Xexact-output-dir',
    '@INPUT0@', '@INPUT1@',
  ]
)

libgrammar = static_library('grammar',
  sources: [src_grammar],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_antlr4],
)

idep_grammar = declare_dependency(
  sources : [src_grammar],
  link_with : [libgrammar],
)

## GTest Wrap Subproject
gtest_proj = subproject('gtest')

## Read build files from subdirectories
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
