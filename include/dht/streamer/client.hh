#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.hh"

#include "streamer.grpc.pb.h"

using dibibase::grpc::Query;
using dibibase::grpc::Result;
using dibibase::grpc::StreamerRPC;
using grpc::Status;

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

    logger.info("gRPC Error(%d): %s", status.error_code(),
                status.error_message().c_str());
    return "Error"; // TODO: change to optional
  }

private:
  std::unique_ptr<StreamerRPC::Stub> stub;

  Logger logger = Logger::make();
};

} // namespace dibibase::dht
