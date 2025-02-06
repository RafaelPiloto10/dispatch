#ifndef BROKER_H
#define BROKER_H

#include "proto/dispatch.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <queue>

class Broker {
public:
  Broker(size_t capacity);

  absl::StatusOr<std::unique_ptr<Job>> GetJob(int32_t preferred_nice);
  absl::Status AddJob(std::unique_ptr<Job> job);

private:
  const size_t capacity_;
  size_t size_;
  absl::Mutex mu_;

  class JobPriorityComparator {
  public:
    bool operator()(const Job &a, const Job &b) const {
      // Move lower values to the front of the queue (nicer jobs allow other
      // jobs to run first)
      return a.nice() > b.nice();
    }
  };
  std::priority_queue<Job, std::vector<Job>, JobPriorityComparator> queue_;
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
