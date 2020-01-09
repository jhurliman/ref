load("//:common.bzl", "REF_COPTS")

cc_library(
    name = "opencv_video_driver_lib",
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
        "@opencv//:core",
        "@opencv//:videoio",
        "//lib/messages:generated_msgs",
        "//lib/node:node_lib",
    ],
    copts = REF_COPTS,
)
