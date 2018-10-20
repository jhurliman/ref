load("//:common.bzl", "REF_COPTS")

cc_library(
    name = "node_lib",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@jsoncpp",
        "//lib/core:core_lib",
        "//lib/messages:messages_lib",
        "//third_party:third_party_lib",
    ],
    copts = REF_COPTS,
)

cc_test(
    name = "node_test",
    srcs = glob(["test/**/*.cpp", "test/**/*.h"]),
    data = glob(["test/data/**/*"]),
    deps = [
        "@googletest//:gtest_main",
        ":node_lib",
    ],
    copts = REF_COPTS,
)
