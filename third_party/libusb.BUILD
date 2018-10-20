
COMMON_HDRS = [
    "libusb/libusb.h",
]

COMMON_SRCS = [
    "config.h",

    "libusb/hotplug.h",
    "libusb/libusbi.h",
    "libusb/version.h",
    "libusb/version_nano.h",

    "libusb/core.c",
    "libusb/descriptor.c",
    "libusb/hotplug.c",
    "libusb/io.c",
    "libusb/sync.c",
    "libusb/strerror.c",
]

LINUX_SRCS = [
    # FIXME
]

WINDOWS_SRCS = [
    # FIXME
]

DARWIN_SRCS = [
    "libusb/os/darwin_usb.c",
    "libusb/os/darwin_usb.h",
    "libusb/os/poll_posix.c",
    "libusb/os/poll_posix.h",
    "libusb/os/threads_posix.c",
    "libusb/os/threads_posix.h",
]

LINUX_LINKOPTS = [] # FIXME
WINDOWS_LINKOPTS = [] # FIXME
DARWIN_LINKOPTS = ["-lobjc", "-Wl,-framework,IOKit", "-Wl,-framework,CoreFoundation"]

LINUX_CONFIG = """
cat > $@ <<"EOF"
// FIXME
EOF"""

WINDOWS_CONFIG = """
cat > $@ <<"EOF"
// FIXME
EOF"""

DARWIN_CONFIG = """
cat > $@ <<"EOF"
#define DEFAULT_VISIBILITY __attribute__((visibility("default")))
#define ENABLE_LOGGING 1
#define HAVE_DECL_TFD_CLOEXEC 0
#define HAVE_DECL_TFD_NONBLOCK 0
#define HAVE_DLFCN_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_MEMORY_H 1
#define HAVE_POLL_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_STRUCT_TIMESPEC 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define LT_OBJDIR ".libs/"
#define OS_DARWIN 1
#define PACKAGE "libusb"
#define PACKAGE_BUGREPORT "libusb-devel@lists.sourceforge.net"
#define PACKAGE_NAME "libusb"
#define PACKAGE_STRING "libusb 1.0.22"
#define PACKAGE_TARNAME "libusb"
#define PACKAGE_URL "http://libusb.info"
#define PACKAGE_VERSION "1.0.22"
#define POLL_NFDS_TYPE nfds_t
#define STDC_HEADERS 1
#define THREADS_POSIX 1
#define VERSION "1.0.22"
#define _GNU_SOURCE 1
EOF"""

config_setting(
    name = "linux",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
)

config_setting(
    name = "windows",
    constraint_values = [
        "@bazel_tools//platforms:windows",
    ],
)

config_setting(
    name = "darwin",
    constraint_values = [
        "@bazel_tools//platforms:osx",
    ],
)

genrule(
    name = "gen_config",
    outs = ["config.h"],
    cmd = DARWIN_CONFIG,
)

cc_library(
    name = "libusb",
    hdrs = COMMON_HDRS,
    srcs = COMMON_SRCS + select({
        ":linux": LINUX_SRCS,
        ":windows": WINDOWS_SRCS,
        ":darwin": DARWIN_SRCS,
    }),
    copts = [
        '-Wextra',
        '-Wall',
    ],
    includes = [
        ".",
        "libusb",
    ],
    linkopts = select({
        ":linux": LINUX_LINKOPTS,
        ":windows": WINDOWS_LINKOPTS,
        ":darwin": DARWIN_LINKOPTS,
    }),
    visibility = ["//visibility:public"],
)
