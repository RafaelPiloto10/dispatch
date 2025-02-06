# Dispatch

A toy example for what a distributed job queue would look like in C++

## Building & Running

The project is configured using Bazel. To build and run, you should first install bazel.
To run the project, start the broker server: `bazel run //broker:main`.
To create jobs, run a client with `bazel run //client:main`.
To consume jobs, create workers with `bazel run //worker:main`.

## TODO

- Implement scheduling algorithm so that higher nice Jobs are not starved
- Persist life of workers and make them poll the server
  - Consider a push model where the broker load balances amongst workers rather than polling
- Create more realistic "toy" jobs to simulate work load
- Find an distributed example where this project can be used to distribute tasks amongst workers
  - Audio transcription across chunks of bytes
  - LLM completion?
