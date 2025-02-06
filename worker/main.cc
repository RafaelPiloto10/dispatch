#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>

#include "proto/dispatch.grpc.pb.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(std::string, target, "0.0.0.0:3000",
          "Server address that the worker will connect to.");

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  std::string target = absl::GetFlag(FLAGS_target);

  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(target, grpc::InsecureChannelCredentials());

  grpc::ClientContext context;
  GetJobRequest request;
  GetJobResponse response;
  std::unique_ptr<Dispatch::Stub> stub = Dispatch::NewStub(channel);

  grpc::Status status = stub->GetJob(&context, request, &response);
  if (!status.ok()) {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  } else {
    std::cout << "Got job: " << response.job().name() << std::endl;
  }

  return 0;
}
