load("//:common.bzl", "REF_COPTS")

cc_binary(
    name = "camera_record",
    srcs = glob([
        "src/**/*.cpp"
    ]),
    data = glob([
        "config/**/*.jsonc"
    ]),
    deps = [
        "//lib/controller:controller_lib",
        "//lib/messages:messages_lib",
        "//modules/drivers/webcam:webcam_driver_lib",
        "//modules/system/recorder:recorder_lib",
    ],
    copts = REF_COPTS
)
