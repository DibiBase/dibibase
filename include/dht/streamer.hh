#pragma once

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.hh"
#include "query_executer.hh"

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

    response->set_str(QueryExecuter(request->str()).execute());

    return Status::OK;
  }

private:
  Logger logger = Logger::make();
};

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

class StreamerService {
public:
  void run_server(std::string server_str) {
    StreamerServer service;

    ::grpc::EnableDefaultHealthCheckService(true);
    ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_str, ::grpc::InsecureServerCredentials());
    builder.RegisterService((::grpc::Service *)&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    logger.info("Server listening on %s", server_str.c_str());

    server->Wait();
  }

private:
  Logger logger = Logger::make();
};

} // namespace dibibase::dht
