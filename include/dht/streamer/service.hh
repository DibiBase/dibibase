#pragma once

#include "common.hh"

#include "server.hh"

using namespace grpc;

namespace dibibase::dht {

class StreamerService {
public:
  void run_server(std::string server_str) {
    StreamerServer service;

    EnableDefaultHealthCheckService(true);
    reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_str, InsecureServerCredentials());
    builder.RegisterService((Service *)&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    logger.info("Server listening on %s", server_str.c_str());

    server->Wait();
  }

private:
  Logger logger = Logger::make();
};

} // namespace dibibase::dht
