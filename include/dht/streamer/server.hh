#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.hh"
#include "dht/local_query_executor.hh"

#include "streamer.grpc.pb.h"

using namespace dibibase::streamer;
using namespace grpc;

namespace dibibase::dht {

class StreamerServer final : public StreamerRPC::Service {
public:
  StreamerServer() {}

  Status execute(ServerContext *context, const Query *request,
                 Result *response) override {
    logger.info("Received query: %s", request->str().c_str());

    std::shared_ptr<Statement> stmt = StatementParser(request->str()).process();
    response->set_str(LocalQueryExecutor(stmt).execute());

    return Status::OK;
  }

  Status health_check(ServerContext *context, const Nothing *request,
                 Health *response) override {
    logger.info("Health Check: %s", context->peer().c_str());

    response->set_ok(true);
    response->set_ready(true);

    return Status::OK;
  }

private:
  Logger logger = Logger::make();
};

} // namespace dibibase::dht
