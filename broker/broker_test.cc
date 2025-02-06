#include "broker/broker.h"
#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>
#include <thread>

class BrokerTest : public testing::Test {
protected:
  const int capacity_ = 100;

  BrokerTest() : broker_(capacity_) {}

  Broker broker_;
};

TEST_F(BrokerTest, InsertJobIsOk) {
  Job job;
  EXPECT_TRUE(broker_.AddJob(std::make_unique<Job>(std::move(job))).ok());
}

TEST_F(BrokerTest, GetJobIsOk) {
  Job job;
  job.set_name("job-name");
  *job.add_params() = "param-1";
  job.set_nice(1);

  Job want;
  want.CopyFrom(job);

  EXPECT_TRUE(broker_.AddJob(std::make_unique<Job>(std::move(job))).ok());

  absl::StatusOr<std::unique_ptr<Job>> got = broker_.GetJob();
  EXPECT_TRUE(got.ok());

  EXPECT_TRUE(
      google::protobuf::util::MessageDifferencer::Equals(want, *got->get()));
}

TEST_F(BrokerTest, EmptyQueueOk) {
  absl::StatusOr<std::unique_ptr<Job>> got = broker_.GetJob();
  EXPECT_FALSE(got.ok());
}

TEST_F(BrokerTest, CapacitySafelyManagesEmptyQueue) {
  // Queue is empty to start
  absl::StatusOr<std::unique_ptr<Job>> got = broker_.GetJob();
  EXPECT_FALSE(got.ok());

  Job job;
  EXPECT_TRUE(broker_.AddJob(std::make_unique<Job>(std::move(job))).ok());
  got = broker_.GetJob();
  EXPECT_TRUE(got.ok());

  // Should return an error on an empty queue
  got = broker_.GetJob();
  EXPECT_FALSE(got.ok());
}

TEST_F(BrokerTest, ThreadSafeAdd) {
  std::vector<std::unique_ptr<std::thread>> threads;
  for (int i = 0; i < capacity_; i++) {
    threads.push_back(std::make_unique<std::thread>(std::thread([this]() {
      Job job;
      absl::Status status =
          broker_.AddJob(std::make_unique<Job>(std::move(job)));
      EXPECT_TRUE(status.ok()) << status.message();
    })));
  }

  for (std::unique_ptr<std::thread> &thread : threads) {
    thread->join();
  }

  threads.clear();

  for (int i = 0; i < capacity_; i++) {
    threads.push_back(std::make_unique<std::thread>(std::thread([this]() {
      Job job;
      EXPECT_TRUE(broker_.GetJob().ok());
    })));
  }

  for (std::unique_ptr<std::thread> &thread : threads) {
    thread->join();
  }

  EXPECT_FALSE(broker_.GetJob().ok());
}

TEST_F(BrokerTest, GetNiceJob) {
  std::vector<Job> jobs;
  for (int i = 19; i >= -20; i--) {
    Job job;
    job.set_nice(i);
    EXPECT_TRUE(broker_.AddJob(std::make_unique<Job>(std::move(job))).ok());
  }

  // We should get jobs with lower nice values first!
  for (int i = -20; i <= 19; i++) {
    absl::StatusOr<std::unique_ptr<Job>> job = broker_.GetJob();
    EXPECT_TRUE(job.ok());
    EXPECT_EQ(job->get()->nice(), i);
  }
}
