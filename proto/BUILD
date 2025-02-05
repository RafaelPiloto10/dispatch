load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@com_github_grpc_grpc//bazel:grpc_build_system.bzl", "grpc_proto_library")
load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_grpc_library", "py_proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
  name = "dispatch_proto",
  srcs = [
    "dispatch.proto"
  ],
)

cc_proto_library(
  name = "dispatch_cc_proto",
  deps = [":dispatch_proto"],
)

cc_grpc_library(
  name = "dispatch_cc_grpc",
  srcs = ["dispatch_proto"],
  grpc_only = True,
  deps = ["dispatch_cc_proto"],
)
