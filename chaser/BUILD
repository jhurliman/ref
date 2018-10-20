
cc_binary(
    name = "chaser",
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
    copts = [
        '-std=c++17',
        '-stdlib=libc++',
        '-flto=thin',
        '-O3',
        '-Wextra',
        '-Wall',
    ],
)
