load("//:common.bzl", "REF_COPTS")

cc_library(
    name = "recorder_lib",
    srcs = glob([
        "src/**/*.cpp"
    ]),
    hdrs = glob([
        "include/**/*.h"
    ]),
    includes = [
        "include",
    ],
    deps = [
        "//lib/core:core_lib",
        "//lib/messages:generated_msgs",
        "//lib/messages:messages_lib",
        "//lib/node:node_lib",
    ],
    copts = REF_COPTS,
)
