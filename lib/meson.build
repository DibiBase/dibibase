dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cc',
]
dibibaselib_api_srcs = [dibibaselib_api_cql3_srcs]

# dibibaselib_lang_cql3_srcs = [
#   'lang/cql3/CqlLexer.cc',
#   'lang/cql3/CqlParser.cc',
# ]
# dibibaselib_lang_srcs = [dibibaselib_lang_cql3_srcs]

dibibaselib_util_srcs = [
  'util/logger.cc',
]

dibibaselib_db_srcs = [
  'db/db.cc',
  'db/sstable_builder.cc',
  'db/sstable_files.cc',
]

dibibaselib_srcs = [
  dibibaselib_api_srcs,
  # dibibaselib_lang_srcs,
  dibibaselib_util_srcs,
  dibibaselib_db_srcs,
]

dibibaselib = shared_library('dibibase',
  sources: dibibaselib_srcs,
  cpp_args : ['-DBUILDING_DIBIBASE'],
  include_directories : inc,
  gnu_symbol_visibility : 'hidden',
  install : true,
)
