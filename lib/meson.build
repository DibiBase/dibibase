dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cc',
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
  'io/compaction.cc',
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

dibibaselib = shared_library('dibibase',
  sources: dibibaselib_srcs,
  cpp_args : ['-DBUILDING_DIBIBASE'],
  include_directories : inc,
  gnu_symbol_visibility : 'hidden',
  install : true,
)
