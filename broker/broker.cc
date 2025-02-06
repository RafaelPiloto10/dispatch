#include "broker.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

Broker::Broker(size_t capacity) : capacity_(capacity) { size_ = 0; }

absl::StatusOr<std::unique_ptr<Job>> Broker::GetJob(int32_t min_nice) {
  absl::MutexLock lock{&mu_};

  if (size_ == 0) {
    return absl::InternalError("Queue is empty");
  }
  size_ -= 1;
  std::unique_ptr<Job> job = std::make_unique<Job>(std::move(queue_.top()));
  queue_.pop();
  return job;
}

absl::Status Broker::AddJob(std::unique_ptr<Job> job) {
  absl::MutexLock lock{&mu_};

  if (size_ == capacity_) {
    return absl::InternalError(
        absl::StrFormat("Broker reached max capacity of %d jobs", capacity_));
  }

  size_ += 1;
  queue_.push(*job.get());
  return absl::OkStatus();
}

DispatchServiceImpl::DispatchServiceImpl() : broker_(/*capacity=*/5){};

grpc::Status DispatchServiceImpl::CreateJob(grpc::ServerContext *context,
                                            const CreateJobRequest *request,
                                            CreateJobResponse *response) {
  std::vector<std::string> params = {request->job().params().begin(),
                                     request->job().params().end()};
  std::unique_ptr<Job> job(request->job().New());
  job->CopyFrom(request->job());
  absl::Status status = broker_.AddJob(std::move(job));
  if (!status.ok()) {
    return grpc::Status(grpc::StatusCode::INTERNAL,
                        std::string(status.message()));
  }
  return grpc::Status::OK;
}

grpc::Status DispatchServiceImpl::GetJob(grpc::ServerContext *context,
                                         const GetJobRequest *request,
                                         GetJobResponse *response) {
  absl::StatusOr<std::unique_ptr<Job>> job = broker_.GetJob(request->nice());
  if (job.ok()) {
    response->CopyFrom(*job->get());
    return grpc::Status::OK;
  }
  return grpc::Status(grpc::StatusCode::INTERNAL,
                      std::string(job.status().message()));
}
