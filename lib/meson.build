dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cc',
  'api/cql3/frame.cc',
  'api/cql3/server_responce.cc',
  'api/cql3/query_result.cc'
]
dibibaselib_api_srcs = [dibibaselib_api_cql3_srcs]

dibibaselib_catalog_srcs = [
  'catalog/data.cc',
  'catalog/record.cc',
  'catalog/schema.cc',
  'catalog/table.cc',
]

dibibaselib_db_srcs = [
  'db/database.cc',
  'db/index_page.cc',
  'db/table_manager.cc',
]

dibibaselib_io_srcs = [
  'io/disk_manager.cc',
  'io/table_builder.cc',
]

dibibaselib_mem_srcs = [
  'mem/summary.cc',
]

dibibaselib_util_srcs = [
  'util/logger.cc',
  'util/buffer.cc',
]

dibibaselib_srcs = [
  dibibaselib_api_srcs,
  dibibaselib_catalog_srcs,
  dibibaselib_db_srcs,
  dibibaselib_io_srcs,
  dibibaselib_mem_srcs,
  dibibaselib_util_srcs,
]
if meson.version().version_compare('>=0.56.0')
  project_source_root = meson.project_source_root()
else
  project_source_root = meson.source_root()
endif
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
dibibaselib = shared_library('dibibase',
  sources: dibibaselib_srcs,
  cpp_args : ['-DBUILDING_DIBIBASE'],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_antlr4, idep_grammar],
  gnu_symbol_visibility : 'hidden',
  install : true,
)
