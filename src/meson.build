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



thread_dep = dependency('threads')



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
executable('streamer',
  sources: ['streamer.cc', src_pb, src_grpc,],
  include_directories : [inc, '/usr/include/antlr4-runtime'],
  dependencies: [dep_grpc, yaml_cpp, dep_antlr4, idep_grammar],
  link_with : [dibibaselib],
  install : true,
)
main_srcs = [
  'main.cc',
  src_pb,
  src_grpc,
]
executable('dibibase',
  sources: 'main.cc',
  include_directories : inc,
  link_with : dibibaselib,
  dependencies: [thread_dep],
  install : true,
)