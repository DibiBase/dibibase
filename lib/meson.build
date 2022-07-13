protoc = find_program('protoc')
protobuf = dependency('protobuf')

grpc_cpp = find_program('grpc_cpp_plugin')

grpc = dependency('grpc', required: false)
if not grpc.found()
  # find the lib without pkg-config
  grpc = cppc.find_library('grpc')
endif

grpcpp = dependency('grpc++', required: false)
if not grpcpp.found()
  # find the lib without pkg-config
  grpcpp = cppc.find_library('grpc++')
endif

grpc_reflection = cppc.find_library('grpc++_reflection', required: false)

threadlibs = dependency('threads')
cmake = import('cmake')
yaml_cpp_proj = cmake.subproject('yaml_cpp')
yaml_cpp = yaml_cpp_proj.dependency('yaml_cpp')
if meson.version().version_compare('>=0.56.0')
  project_source_root = meson.project_source_root()
else
  project_source_root = meson.source_root()
endif

protoc_gen = generator(protoc,
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
  arguments : ['--proto_path=' + project_source_root + '/protos',
    '--cpp_out=@BUILD_DIR@',
    '@INPUT@'])

grpc_gen = generator(protoc,
  output    : ['@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
  arguments : ['--proto_path=' + project_source_root + '/protos',
    '--grpc_out=@BUILD_DIR@',
    '--plugin=protoc-gen-grpc=' + grpc_cpp.path(),
    '@INPUT@'])
src_streamer_pb = protoc_gen.process(
  project_source_root + '/protos/streamer.proto',
  preserve_path_from : project_source_root + '/protos')

src_streamer_grpc = grpc_gen.process(
  project_source_root + '/protos/streamer.proto',
  preserve_path_from : project_source_root + '/protos')
src_pb = [
  src_streamer_pb
]

src_grpc = [
  src_streamer_grpc
]

dep_grpc = [
  grpc,
  grpc_reflection,
  grpcpp,
  protobuf,
  threadlibs,
]

dibibaselib_api_prom_srcs = [
  'api/prom_endp/http_server.cc',
  'api/prom_endp/http_message.cc'
]
dibibaselib_api_cql3_srcs = [
  'api/cql3/server.cc',
  'api/cql3/frame.cc',
  'api/cql3/server_response.cc',
  'api/cql3/query_result.cc',
  'api/cql3/fake_meta.cc'
]


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

dibibaselib_api_srcs = [
  dibibaselib_api_prom_srcs,
  dibibaselib_api_cql3_srcs
]

dibibaselib_dht_srcs = [
  'dht/murmur_hash.cc',
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
  dibibaselib_dht_srcs,
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

thread_dep = dependency('threads')




dibibaselib = shared_library('dibibase',
  sources: [dibibaselib_srcs,src_pb,src_grpc],
  cpp_args : ['-DBUILDING_DIBIBASE'],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_grpc,yaml_cpp, dep_antlr4, idep_grammar, thread_dep],
  gnu_symbol_visibility : 'hidden',
  install : true,
)