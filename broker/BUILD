cc_library(
    name = "broker",
    srcs = [
        "broker.cc",
    ],
    hdrs = [
        "broker.h",
    ],
    deps = [
        "//proto:dispatch_cc_grpc",
        "//proto:dispatch_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
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

cc_test(
    name = "broker_test",
    srcs = [
        "broker_test.cc",
    ],
    deps = [
        ":broker",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
