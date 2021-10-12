dibibaselib_args = ['-DBUILDING_DIBIBASE']

dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cpp',
]
dibibaselib_api_srcs = [dibibaselib_api_cql3_srcs]

dibibaselib_util_srcs = [
  'util/logger.cpp',
]

dibibaselib_srcs = [dibibaselib_api_srcs, dibibaselib_util_srcs]

dibibaselib = shared_library('dibibase',
  sources: dibibaselib_srcs,
  cpp_args : dibibaselib_args,
  include_directories : inc,
  gnu_symbol_visibility : 'hidden',
  install : true,
)
