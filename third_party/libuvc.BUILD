
CONFIG = """
cat > $@ <<"EOF"
#pragma once

#define LIBUVC_VERSION_MAJOR 0
#define LIBUVC_VERSION_MINOR 0
#define LIBUVC_VERSION_PATCH 6
#define LIBUVC_VERSION_STR "0.0.6"
#define LIBUVC_VERSION_INT                      \
  ((0 << 16) |             \
   (0 << 8) |              \
   (6))

#define LIBUVC_VERSION_GTE(major, minor, patch)                         \
  (LIBUVC_VERSION_INT >= (((major) << 16) | ((minor) << 8) | (patch)))

#define LIBUVC_HAS_JPEG 1
EOF"""

genrule(
    name = "gen_config",
    outs = ["include/libuvc/libuvc_config.h"],
    cmd = CONFIG,
)

cc_library(
    name = "libuvc",
    hdrs = [
        "include/libuvc/libuvc.h",
    ],
    srcs = [
        "include/utlist.h",
        "include/libuvc/libuvc_config.h",
        "include/libuvc/libuvc_internal.h",

        "src/ctrl.c",
        "src/ctrl-gen.c",
        "src/device.c",
        "src/diag.c",
        "src/frame.c",
        "src/frame-mjpeg.c",
        "src/init.c",
        "src/stream.c",
        "src/misc.c",
    ],
    copts = [
        '-Wextra',
        '-Wall',
    ],
    includes = [
        "include",
    ],
    deps = [
        "@libjpeg",
        "@libusb",
    ],
    visibility = ["//visibility:public"],
)
