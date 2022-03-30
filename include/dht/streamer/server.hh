#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.hh"
#include "dht/query_executor.hh"

#include "streamer.grpc.pb.h"

using dibibase::grpc::Query;
using dibibase::grpc::Result;
using dibibase::grpc::StreamerRPC;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace dibibase::dht {

class StreamerServer final : public StreamerRPC::Service {
public:
  StreamerServer() {}

  Status execute(ServerContext *context, const Query *request,
                 Result *response) override {
    logger.info("Received query: %s", request->str().c_str());

    response->set_str(QueryExecutor(request->str()).execute());

    return Status::OK;
  }

private:
  Logger logger = Logger::make();
};


} // namespace dibibase::dht
