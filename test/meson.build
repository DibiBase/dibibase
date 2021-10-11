gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')

test_api_cql3_server = executable('api_cql3_server_test',
  sources : 'api/cql3/server_test.cpp',
  include_directories : inc,
  link_with : dibibaselib)
test('api/cql3/server', test_api_cql3_server)

test_gtest = executable('gtest_test',
  sources : 'gtest_test.cpp',
  include_directories : inc,
  link_with : dibibaselib,
  dependencies : [
    gtest_dep,
    gmock_dep
  ],
)
test('gtest', test_gtest)
