#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <memory>
#include <string>
#include <vector>

#include "catalog/record.hh"
#include "util/logger.hh"

#include "streamer.grpc.pb.h"
#include "streamer.pb.h"

using dibibase::catalog::Record;
using dibibase::grpc::Query;
using dibibase::grpc::Result;
using dibibase::grpc::StreamerRPC;
using dibibase::util::Logger;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

Logger logger = Logger::make();

namespace dibibase::dht {

class StreamerServer final : public StreamerRPC::Service {
public:
  StreamerServer() {}
  StreamerServer(std::shared_ptr<QueryProcessor> query_processor)
      : query_processor_(query_processor){};

  Status execute(ServerContext *context, const Query *request,
                 Result *response) override {
    logger.info("Received query: {}", request);

    std::vector<Record> records = m_query_processor.process(request);
    for (auto &record : records) {
      response->add_records(record.to_string());
    }

    return Status::OK;
  }

private:
  std::shared_ptr<QueryProcessor> m_query_processor;
};

class StreamerClient {
public:
  StreamerClient(std::shared_ptr<::grpc::Channel> channel)
      : stub_(StreamerRPC::NewStub(channel)){};

  std::vector<Record> execute(const Query &request) {
    ::grpc::ClientContext context;
    Result response;
    Status status = stub_->execute(&context, request, &response);

    if (status.ok()) {
      std::vector<Record> records;
      for (auto &record : response.records()) {
        records.push_back(Record::from(record));
      }
      return records;
    }

    logger.info(status.error_message(), ": ", status.error_code());
    return {};
  }

private:
  std::unique_ptr<StreamerRPC::Stub> stub_;
};

void run_server(int portno) {
  std::string server_address("0.0.0.0:" + std::to_string(portno));
  StreamerServer service;

  ::grpc::EnableDefaultHealthCheckService(true);
  ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  logger.info("Server listening on ", server_address);

  server->Wait();
}

} // namespace dibibase::dhts