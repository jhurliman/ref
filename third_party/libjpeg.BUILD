
CONFIG = """
cat > $@ <<"EOF"
#define HAVE_PROTOTYPES 1
#define HAVE_UNSIGNED_CHAR 1
#define HAVE_UNSIGNED_SHORT 1
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
#define HAVE_LOCALE_H 1

#ifdef _WIN32
#ifndef __RPCNDR_H__            /* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE                   /* in case these macros already exist */
#define FALSE   0               /* values of boolean */
#endif
#ifndef TRUE
#define TRUE    1
#endif
#define HAVE_BOOLEAN            /* prevent jmorecfg.h from redefining it */
#endif

#ifdef JPEG_INTERNALS
#define INLINE __inline__
#endif

#ifdef JPEG_CJPEG_DJPEG
#define BMP_SUPPORTED           /* BMP image file format */
#define GIF_SUPPORTED           /* GIF image file format */
#define PPM_SUPPORTED           /* PBMPLUS PPM/PGM image file format */
#define TARGA_SUPPORTED         /* Targa image file format */
#endif
EOF"""

genrule(
    name = "gen_config",
    outs = ["jconfig.h"],
    cmd = CONFIG,
)

cc_library(
    name = "libjpeg",
    hdrs = [
        "jpeglib.h",
    ],
    srcs = [
        "cderror.h",
        "cdjpeg.h",
        "jconfig.h",
        "jdct.h",
        "jerror.h",
        "jinclude.h",
        "jmemsys.h",
        "jmorecfg.h",
        "jpegint.h",
        "jpeglib.h",
        "jversion.h",
        "transupp.h",

        "jaricom.c",
        "jcapimin.c",
        "jcapistd.c",
        "jcarith.c",
        "jccoefct.c",
        "jccolor.c",
        "jcdctmgr.c",
        "jchuff.c",
        "jcinit.c",
        "jcmainct.c",
        "jcmarker.c",
        "jcmaster.c",
        "jcomapi.c",
        "jcparam.c",
        "jcprepct.c",
        "jcsample.c",
        "jctrans.c",
        "jdapimin.c",
        "jdapistd.c",
        "jdarith.c",
        "jdatadst.c",
        "jdatasrc.c",
        "jdcoefct.c",
        "jdcolor.c",
        "jddctmgr.c",
        "jdhuff.c",
        "jdinput.c",
        "jdmainct.c",
        "jdmarker.c",
        "jdmaster.c",
        "jdmerge.c",
        "jdpostct.c",
        "jdsample.c",
        "jdtrans.c",
        "jerror.c",
        "jfdctflt.c",
        "jfdctfst.c",
        "jfdctint.c",
        "jidctflt.c",
        "jidctfst.c",
        "jidctint.c",
        "jquant1.c",
        "jquant2.c",
        "jutils.c",
        "jmemmgr.c",
        "jmemnobs.c",
    ],
    copts = [
        '-Wextra',
        '-Wall',
    ],
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"],
)
