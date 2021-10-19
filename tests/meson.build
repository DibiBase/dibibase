gtest_dep = gtest_proj.get_variable('gtest_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')

test_api_cql3_server = executable('api_cql3_server_test',
  sources : 'api/cql3/server_test.cc',
  include_directories : inc,
  link_with : dibibaselib)
test('cql3/server', test_api_cql3_server, suite: 'api')

test_gtest = executable('gtest_test',
  sources : 'gtest_test.cc',
  include_directories : inc,
  link_with : dibibaselib,
  dependencies : [
    gtest_dep,
    gmock_dep
  ],
)
test('gtest', test_gtest, suite: 'general')
