#ifndef BROKER_H
#define BROKER_H

#include "proto/dispatch.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <queue>
#include <string>

class Job {
public:
  Job(std::string name, std::vector<std::string> params);

  std::string name();

  std::vector<std::string> params();

private:
  std::string name_;
  std::vector<std::string> params_;
};

class Broker {
public:
  Broker(size_t capacity);

  absl::StatusOr<std::unique_ptr<Job>> GetJob();
  absl::Status AddJob(std::unique_ptr<Job> job);

private:
  const size_t capacity_;
  size_t size_;
  std::queue<std::unique_ptr<Job>> queue_;
  absl::Mutex mu_;
};

class DispatchServiceImpl final : public Dispatch::Service {
public:
  grpc::Status CreateJob(grpc::ServerContext *context,
                         const CreateJobRequest *request,
                         CreateJobResponse *response) override;

  grpc::Status GetJob(grpc::ServerContext *context,
                      const GetJobRequest *request,
                      GetJobResponse *response) override;

  DispatchServiceImpl();

private:
  Broker broker_;
};

#endif
