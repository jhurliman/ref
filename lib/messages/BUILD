load("//:common.bzl", "REF_COPTS")
load("@//third_party:flatbuffers.bzl", "cc_flatbuffers_library")

cc_flatbuffers_library(
    name = "generated_msgs",
    srcs = glob(["definitions/**/*.fbs"]),
    include = "definitions",
)

cc_library(
    name = "messages_lib",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@flatbuffers",
        "//lib/core:core_lib",
        ":generated_msgs",
    ],
    copts = REF_COPTS,
)

cc_test(
    name = "messages_test",
    srcs = glob(["test/**/*.cpp"]),
    data = glob(["test/data/**/*"]),
    deps = [
        "@googletest//:gtest_main",
        ":generated_msgs",
        ":messages_lib",
    ],
    copts = REF_COPTS,
)
