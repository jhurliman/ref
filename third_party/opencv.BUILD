
cc_library(
    name = "core",
    srcs = glob([
        "modules/core/src/**/*.cpp",
    ]),
    hdrs = glob([
        "modules/core/src/**/*.hpp",
        "modules/core/include/**/*.hpp",
    ]) + [
        ":module_includes",
        ":cvconfig",
        ":version_string",
    ],
    # FIXME: Make this compatible with Windows
    linkopts = ["-pthread", "-lz"],
    includes = ["modules/core/include"],
)

genrule(
    name = "module_includes",
    cmd = "echo '#define HAVE_OPENCV_CORE' > $@",
    outs = ["opencv2/opencv_modules.hpp"],
)

genrule(
    name = "cvconfig",
    outs = ["cvconfig.h"],
    cmd = """
cat > $@ <<"EOF"
// IJG JPEG
#define HAVE_JPEG

// PNG
#define HAVE_PNG

// Compile for 'real' NVIDIA GPU architectures
#define CUDA_ARCH_BIN ""

// NVIDIA GPU features are used
#define CUDA_ARCH_FEATURES ""

// Compile for 'virtual' NVIDIA PTX architectures
#define CUDA_ARCH_PTX ""
EOF"""
)

genrule(
    name = "version_string",
    outs = ["version_string.inc"],
    cmd = """
cat > $@ <<"EOF"
"\\n"
)
