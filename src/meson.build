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

src_pb = protoc_gen.process(
  project_source_root + '/protos/helloworld.proto',
  preserve_path_from : project_source_root + '/protos')

src_grpc = grpc_gen.process(
  project_source_root + '/protos/helloworld.proto',
  preserve_path_from : project_source_root + '/protos')

dep_grpc = [
  grpc,
  grpc_reflection,
  grpcpp,
  protobuf,
  threadlibs,
]

executable('greeter_client',
  sources: ['greeter_client.cc', src_pb, src_grpc],
  include_directories: inc,
  dependencies: dep_grpc,
  install: true,
)

executable('greeter_server',
  sources: ['greeter_server.cc', src_pb, src_grpc],
  include_directories: inc,
  dependencies: dep_grpc,
  install: true,
)

executable('logger',
  sources: 'logger.cc',
  include_directories: inc,
  link_with : dibibaselib,
  install: false,
)

executable('lang',
  sources: ['lang.cc'],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_antlr4, idep_grammar],
  link_with : [dibibaselib],
  install : true,
)

executable('dibibase',
  sources: 'main.cc',
  include_directories : inc,
  link_with : dibibaselib,
  install : true,
)
