cc_library(
  name = "broker",
  srcs = [
    "broker.cc",
  ],
  hdrs = [
    "broker.h",
  ],
  deps = [
    "//proto:dispatch_cc_proto",
    "//proto:dispatch_cc_grpc",
    "@com_github_grpc_grpc//:grpc++",
  ]
)

cc_binary(
  name = "main",
  srcs = [
    "main.cc",
  ],
  deps = [
    ":broker",
    "@com_github_grpc_grpc//:grpc++",
  ],
)

