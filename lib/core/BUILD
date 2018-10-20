load("//:common.bzl", "REF_COPTS")

cc_library(
    name = "core_lib",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = ["//third_party:third_party_lib"],
    copts = REF_COPTS,
)

cc_test(
    name = "core_test",
    srcs = glob(["test/**/*.cpp"]),
    deps = [
        "@googletest//:gtest_main",
        ":core_lib",
    ],
    copts = REF_COPTS,
)
