#include "broker.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

Job::Job(std::string name, std::vector<std::string> params)
    : name_(name), params_(params) {}
std::string Job::name() { return name_; }
std::vector<std::string> Job::params() { return params_; }

Broker::Broker(size_t capacity) : capacity_(capacity) { size_ = 0; }

absl::StatusOr<std::unique_ptr<Job>> Broker::GetJob() {
  absl::MutexLock lock{&mu_};

  if (size_ == 0) {
    return absl::InternalError("Queue is empty");
  }
  size_ -= 1;
  std::unique_ptr<Job> job = std::move(queue_.front());
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
  queue_.push(std::move(job));
  return absl::OkStatus();
}

DispatchServiceImpl::DispatchServiceImpl() : broker_(5){};

grpc::Status DispatchServiceImpl::CreateJob(grpc::ServerContext *context,
                                            const CreateJobRequest *request,
                                            CreateJobResponse *response) {
  std::vector<std::string> params = {request->params().begin(),
                                     request->params().end()};
  absl::Status status =
      broker_.AddJob(std::make_unique<Job>(request->name(), params));
  if (!status.ok()) {
    return grpc::Status(grpc::StatusCode::INTERNAL,
                        std::string(status.message()));
  }
  return grpc::Status::OK;
}

grpc::Status DispatchServiceImpl::GetJob(grpc::ServerContext *context,
                                         const GetJobRequest *request,
                                         GetJobResponse *response) {

  absl::StatusOr<std::unique_ptr<Job>> job = broker_.GetJob();
  if (job.ok()) {
    response->set_name(job->get()->name());
    for (std::string param : job->get()->params()) {
      *response->add_params() = param;
    }
    return grpc::Status::OK;
  }
  return grpc::Status(grpc::StatusCode::INTERNAL,
                      std::string(job.status().message()));
}
