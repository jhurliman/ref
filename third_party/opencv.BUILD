
# FIXME: Make this compatible with Windows
LINKOPTS = ["-pthread", "-lz"]

cc_library(
    name = "core",
    srcs = glob([
        "modules/core/src/**/*.cpp",
    ]),
    hdrs = glob([
        "include/opencv2/opencv.hpp",
        "modules/core/include/opencv2/core.hpp",
        "modules/core/include/opencv2/core/**/*.h",
        "modules/core/include/opencv2/core/**/*.hpp",
        "modules/core/src/**/*.hpp",
    ]) + [
        ":module_includes",
        ":custom_hal",
        ":cvconfig",
        ":cv_cpu_config",
        ":version_string",
    ],
    copts = ["-D__OPENCV_BUILD=1"],
    linkopts = LINKOPTS,
    includes = ["include", "modules/core/include"],
)

cc_library(
    name = "imgcodecs",
    srcs = glob([
        "modules/imgcodecs/src/**/*.cpp",
        "modules/imgcodecs/src/**/*.h*",
    ]),
    hdrs = glob([
        "modules/imgcodecs/include/opencv2/imgcodecs.hpp",
        "modules/imgcodecs/include/opencv2/imgcodecs/**/*.h",
        "modules/imgcodecs/include/opencv2/imgcodecs/**/*.hpp",
    ]) + [
        ":module_includes",
        ":cvconfig",
        ":cv_cpu_config",
        ":version_string",
    ],
    copts = ["-D__OPENCV_BUILD=1"],
    deps = [":core", ":imgproc"],
    linkopts = LINKOPTS,
    includes = ["modules/imgcodecs/include"],
)

cc_library(
    name = "imgproc",
    srcs = glob([
        "modules/imgproc/src/**/*.cpp",
        "modules/imgproc/src/**/*.h*",
    ]),
    hdrs = glob([
        "modules/imgproc/include/opencv2/imgproc.hpp",
        "modules/imgproc/include/opencv2/imgproc/**/*.h",
        "modules/imgproc/include/opencv2/imgproc/**/*.hpp",
    ]) + [
        ":module_includes",
        ":cvconfig",
        ":cv_cpu_config",
        ":version_string",
    ],
    copts = ["-D__OPENCV_BUILD=1"],
    deps = [":core"],
    linkopts = LINKOPTS,
    includes = ["modules/imgproc/include"],
)

cc_library(
    name = "videoio",
    srcs = [
        "modules/videoio/src/cap.cpp",
        "modules/videoio/src/cap_aravis.cpp",
        "modules/videoio/src/cap_dc1394.cpp",
        "modules/videoio/src/cap_dc1394_v2.cpp",
        # "modules/videoio/src/cap_dshow.cpp",
        # "modules/videoio/src/cap_ffmpeg.cpp",
        # "modules/videoio/src/cap_giganetix.cpp",
        # "modules/videoio/src/cap_gphoto2.cpp",
        # "modules/videoio/src/cap_gstreamer.cpp",
        "modules/videoio/src/cap_images.cpp",
        # "modules/videoio/src/cap_intelperc.cpp",
        # "modules/videoio/src/cap_librealsense.cpp",
        "modules/videoio/src/cap_mfx_common.cpp",
        "modules/videoio/src/cap_mfx_reader.cpp",
        "modules/videoio/src/cap_mfx_writer.cpp",
        "modules/videoio/src/cap_mjpeg_decoder.cpp",
        "modules/videoio/src/cap_mjpeg_encoder.cpp",
        "modules/videoio/src/cap_msmf.cpp",
        # "modules/videoio/src/cap_openni.cpp",
        # "modules/videoio/src/cap_openni2.cpp",
        "modules/videoio/src/cap_pvapi.cpp",
        # "modules/videoio/src/cap_v4l.cpp",
        # "modules/videoio/src/cap_winrt_bridge.cpp",
        # "modules/videoio/src/cap_winrt_capture.cpp",
        # "modules/videoio/src/cap_winrt_video.cpp",
        # "modules/videoio/src/cap_ximea.cpp",
        # "modules/videoio/src/cap_xine.cpp",
        "modules/videoio/src/container_avi.cpp",
        "modules/videoio/src/videoio_c.cpp",
        "modules/videoio/src/videoio_registry.cpp",
    ] + glob([
        "modules/videoio/src/**/*.h*",
    ]),
    hdrs = glob([
        "modules/videoio/include/opencv2/videoio.hpp",
        "modules/videoio/include/opencv2/videoio/**/*.h",
        "modules/videoio/include/opencv2/videoio/**/*.hpp",
    ]) + [
        ":module_includes",
        ":cvconfig",
        ":cv_cpu_config",
        ":version_string",
    ],
    copts = ["-D__OPENCV_BUILD=1"],
    deps = [":core", ":imgcodecs"],
    linkopts = LINKOPTS,
    includes = ["modules/videoio/include"],
)

genrule(
    name = "module_includes",
    outs = ["opencv2/opencv_modules.hpp"],
    cmd = """
cat > $@ <<"EOF"
#define HAVE_OPENCV_CORE
#define HAVE_OPENCV_IMGCODECS
#define HAVE_OPENCV_IMGPROC
#define HAVE_OPENCV_VIDEOIO
EOF"""
)

genrule(
    name = "cvconfig",
    outs = ["cvconfig.h"],
    cmd = """
cat > $@ <<"EOF"
#ifndef OPENCV_CVCONFIG_H_INCLUDED
#define OPENCV_CVCONFIG_H_INCLUDED

#define BUILD_SHARED_LIBS
#define CV_ENABLE_INTRINSICS
#define CUDA_ARCH_BIN ""
#define CUDA_ARCH_FEATURES ""
#define CUDA_ARCH_PTX ""

/* MacOS-specific */
#define HAVE_AVFOUNDATION
#define HAVE_COCOA

#define HAVE_OPENCL
#define HAVE_OPENCL_STATIC
#define HAVE_OPENEXR

/* CUDA/Nvidia-specific */
/* #undef HAVE_CUBLAS */
/* #undef HAVE_CUDA */
/* #undef HAVE_CUFFT */
/* #undef HAVE_NVCUVID */
/* #undef HAVE_NVCUVENC */

/* #undef HAVE_EIGEN */
/* #undef HAVE_FFMPEG */

#define HAVE_INTTYPES_H 1

#define HAVE_JPEG
#define HAVE_PNG
#define HAVE_TIFF

/* #undef HAVE_OPENNI */
/* #undef HAVE_OPENNI2 */
/* #undef HAVE_LIBREALSENSE */

#define HAVE_PTHREAD
#define HAVE_PTHREADS_PF

#define HAVE_LAPACK
#define HAVE_QUIRC
#define OPENCV_TRACE

#if defined(HAVE_XINE)         || \
    defined(HAVE_GSTREAMER)    || \
    defined(HAVE_AVFOUNDATION) || \
    defined(HAVE_FFMPEG)       || \
    defined(HAVE_MSMF)
#define HAVE_VIDEO_INPUT
#endif

#if defined(HAVE_GSTREAMER)    || \
    defined(HAVE_AVFOUNDATION) || \
    defined(HAVE_FFMPEG)       || \
    defined(HAVE_MSMF)
#define HAVE_VIDEO_OUTPUT
#endif

#endif // OPENCV_CVCONFIG_H_INCLUDED
EOF"""
)

genrule(
    name = "cv_cpu_config",
    outs = ["cv_cpu_config.h"],
    cmd = """
cat > $@ <<"EOF"
// OpenCV CPU baseline features

#define CV_CPU_COMPILE_SSE 1
#define CV_CPU_BASELINE_COMPILE_SSE 1

#define CV_CPU_COMPILE_SSE2 1
#define CV_CPU_BASELINE_COMPILE_SSE2 1

#define CV_CPU_COMPILE_SSE3 1
#define CV_CPU_BASELINE_COMPILE_SSE3 1

#define CV_CPU_COMPILE_SSSE3 1
#define CV_CPU_BASELINE_COMPILE_SSSE3 1

#define CV_CPU_COMPILE_SSE4_1 1
#define CV_CPU_BASELINE_COMPILE_SSE4_1 1

#define CV_CPU_BASELINE_FEATURES 0 \
    , CV_CPU_SSE \
    , CV_CPU_SSE2 \
    , CV_CPU_SSE3 \
    , CV_CPU_SSSE3 \
    , CV_CPU_SSE4_1 \

// OpenCV supported CPU dispatched features

#define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
#define CV_CPU_DISPATCH_COMPILE_FP16 1
#define CV_CPU_DISPATCH_COMPILE_AVX 1
#define CV_CPU_DISPATCH_COMPILE_AVX2 1
#define CV_CPU_DISPATCH_COMPILE_AVX512_SKX 1

#define CV_CPU_DISPATCH_FEATURES 0 \
    , CV_CPU_SSE4_2 \
    , CV_CPU_FP16 \
    , CV_CPU_AVX \
    , CV_CPU_AVX2 \
    , CV_CPU_AVX512_SKX \
EOF"""
)

genrule(
    name = "version_string",
    outs = ["version_string.inc"],
    cmd = """
cat > $@ <<"EOF"
"\\n"
EOF"""
)

genrule(
    name = "custom_hal",
    outs = ["custom_hal.hpp"],
    cmd = """
cat > $@ <<"EOF"
#ifndef _CUSTOM_HAL_INCLUDED_
#define _CUSTOM_HAL_INCLUDED_

#endif
EOF"""
)
