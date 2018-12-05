workspace(name = "ref_ws")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "eigen",
    url = "https://bitbucket.org/eigen/eigen/get/f3a22f35b044.tar.gz",
    build_file = "@//:third_party/eigen.BUILD",
    sha256 = "ca7beac153d4059c02c8fc59816c82d54ea47fe58365e8aded4082ded0b820c4",
    strip_prefix = "eigen-eigen-f3a22f35b044",
)

http_archive(
    name = "flatbuffers",
    url = "https://github.com/google/flatbuffers/archive/v1.10.0.tar.gz",
    build_file = "@//:third_party/flatbuffers.BUILD",
    sha256 = "3714e3db8c51e43028e10ad7adffb9a36fc4aa5b1a363c2d0c4303dd1be59a7c",
    strip_prefix = "flatbuffers-1.10.0",
)

http_archive(
    name = "googletest",
    url = "https://github.com/google/googletest/archive/release-1.8.1.tar.gz",
    build_file = "@//:third_party/googletest.BUILD",
    sha256 = "9bf1fe5182a604b4135edc1a425ae356c9ad15e9b23f9f12a02e80184c3a249c",
    strip_prefix = "googletest-release-1.8.1",
)

http_archive(
    name = "jsoncpp",
    url = "https://github.com/open-source-parsers/jsoncpp/archive/1.8.4.tar.gz",
    build_file = "@//:third_party/jsoncpp.BUILD",
    sha256 = "c49deac9e0933bcb7044f08516861a2d560988540b23de2ac1ad443b219afdb6",
    strip_prefix = "jsoncpp-1.8.4",
)

# TODO: Switch to libjpeg-turbo
http_archive(
    name = "libjpeg",
    url = "https://www.ijg.org/files/jpegsrc.v9c.tar.gz",
    build_file = "@//:third_party/libjpeg.BUILD",
    sha256 = "650250979303a649e21f87b5ccd02672af1ea6954b911342ea491f351ceb7122",
    strip_prefix = "jpeg-9c",
)

http_archive(
    name = "libusb",
    url = "https://github.com/libusb/libusb/archive/v1.0.22.tar.gz",
    build_file = "@//:third_party/libusb.BUILD",
    sha256 = "3500f7b182750cd9ccf9be8b1df998f83df56a39ab264976bdb3307773e16f48",
    strip_prefix = "libusb-1.0.22",
)

http_archive(
    name = "libuvc",
    url = "https://github.com/ktossell/libuvc/archive/v0.0.6.tar.gz",
    build_file = "@//:third_party/libuvc.BUILD",
    sha256 = "42175a53c1c704365fdc782b44233925e40c9344fbb7f942181c1090f06e2873",
    strip_prefix = "libuvc-0.0.6",
)

http_archive(
    name = "opencv",
    url = "https://github.com/opencv/opencv/archive/4.0.0-alpha.tar.gz",
    build_file = "@//:third_party/opencv.BUILD",
    strip_prefix = "opencv-4.0.0-alpha",
)
