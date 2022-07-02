#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.hh"

#include "streamer.grpc.pb.h"

using namespace dibibase::streamer;
using namespace grpc;

namespace dibibase::dht {

class StreamerClient {
public:
  StreamerClient(std::shared_ptr<::grpc::Channel> channel)
      : stub(StreamerRPC::NewStub(channel)){};

  std::string execute(const std::string &query) {
    ::grpc::ClientContext context;
    Query request;
    Result response;

    request.set_str(query);
    Status status = stub->execute(&context, request, &response);

    if (status.ok()) {
      return response.str();
    }

    logger.err("gRPC Error(%d): %s", status.error_code(),
                status.error_message().c_str());
    return "Error"; // TODO: change to optional
  }

  bool health_check() {
    ::grpc::ClientContext context;
    Nothing request;
    Health response;

    Status status = stub->health_check(&context, request, &response);

    if (status.ok()) {
      return response.ready();
    }

    logger.info("gRPC Error(%d): %s, HealthCheck Failed: %s", status.error_code(),
                status.error_message().c_str(), context.peer().c_str());
    return false;
  }

private:
  std::unique_ptr<StreamerRPC::Stub> stub;

  Logger logger = Logger::make();
};

} // namespace dibibase::dht
