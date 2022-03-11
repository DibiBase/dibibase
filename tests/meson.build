gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')

test_api_cql3_server = executable('api_cql3_server_test',
  sources : 'api/cql3/server_test.cc',
  include_directories : inc,
  link_with : dibibaselib)
test('cql3/server', test_api_cql3_server, suite: 'api')

gtest_srcs = [
  'gtest_test.cc'
]
util_srcs = [
  'storage-engine/util/buffer_test.cc',
  'storage-engine/util/factory_test.cc',
  'storage-engine/util/logger_test.cc' 
]
io_srcs = [
  'storage-engine/io/disk_manager_test.cc',
  'storage-engine/io/table_builder_test.cc'
]
mem_srcs = [
  'storage-engine/mem/summary_test.cc'
]
catalog_srcs = [
  'storage-engine/catalog/data_test.cc',
  'storage-engine/catalog/schema_test.cc',
  'storage-engine/catalog/record_test.cc',
  'storage-engine/catalog/table_test.cc'  
]
db_srcs = [
  'storage-engine/db/table_manager_test.cc',
  'storage-engine/db/index_page_test.cc',
  'storage-engine/db/database_test.cc'
]
storage_test_srcs = [
  gtest_srcs,
  util_srcs,
  io_srcs,
  mem_srcs,
  catalog_srcs,
  db_srcs
]
test_gtest = executable('gtest_test',
  sources : storage_test_srcs,
  include_directories : inc,
  link_with : dibibaselib,
  dependencies : [
    gtest_dep,
    gmock_dep
  ],
)
test('gtest', test_gtest, suite: 'general')
