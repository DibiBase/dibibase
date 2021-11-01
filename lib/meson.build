dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cc',
]
dibibaselib_api_srcs = [dibibaselib_api_cql3_srcs]

dibibaselib_lang_srcs = ['catalog/types.cc']

dibibaselib_lang_cql3_srcs = [
  'lang/cql3/SelectStatementVisitor.cc',
]
dibibaselib_lang_srcs = [dibibaselib_lang_cql3_srcs]

dibibaselib_util_srcs = [
  'util/logger.cc',
]

dibibaselib_srcs = [
  dibibaselib_api_srcs,
  # dibibaselib_lang_srcs,
  dibibaselib_util_srcs,
]

dibibaselib = shared_library('dibibase',
  sources: dibibaselib_srcs,
  cpp_args : ['-DBUILDING_DIBIBASE'],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_antlr4, idep_grammar],
  gnu_symbol_visibility : 'hidden',
  install : true,
)
