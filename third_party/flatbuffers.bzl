
def _impl(ctx):
    includes = ctx.attr.includes
    inputs = ctx.files.srcs + ctx.files.data
    for dep_target in ctx.attr.deps:
        includes += dep_target.fbs.includes
        inputs += dep_target.fbs.inputs

    # Generate C++ source code
    for i, src in enumerate(ctx.files.srcs):
        out = ctx.outputs.out_cpp[i]
        args = ['--cpp', '--gen-object-api'] + ['-o', out.dirname] + [src.path]

        ctx.action(
            inputs=inputs,
            outputs=[out],
            executable=ctx.executable.flatc,
            arguments=args,
            mnemonic="GenFlatbuffer",
        )

    # Generate compiled schema files
    for i, src in enumerate(ctx.files.srcs):
        out = ctx.outputs.out_schema[i]
        args = ['--binary', '--schema'] + ['-o', out.dirname] + [src.path]

        ctx.action(
            inputs=inputs,
            outputs=[out],
            executable=ctx.executable.flatc,
            arguments=args,
            mnemonic="GenFlatbufferSchema",
        )

    return struct(
        fbs=struct(
            includes=includes,
            inputs=inputs,
        )
    )

_fbs_gen = rule(
    attrs={
        "srcs": attr.label_list(allow_files=True),
        "deps": attr.label_list(providers = ["fbs"]),
        "data": attr.label_list(allow_files=True),
        "includes": attr.string_list(),
        "flatc": attr.label(executable=True,
                            single_file=True,
                            cfg='host',
                            default=Label("@flatbuffers//:flatc")),
        "out_cpp": attr.output_list(),
        "out_schema": attr.output_list(),
    },
    output_to_genfiles=True,
    implementation=_impl,
)

def cc_flatbuffers_library(
        name,
        srcs=[],
        deps=[],
        data=[],
        include=None,
        **kargs):
    """Bazel rule to create a C++ flatbuffers library from .fbs source files"""

    includes = []
    if include != None:
      includes = [include]

    out_cpp = [s.replace('.fbs', '_generated.h') for s in srcs]
    out_schema = [s.replace('.fbs', '.bfbs') for s in srcs]

    _fbs_gen(
        name=name + "_genfbs",
        srcs=srcs,
        deps=[s + "_genfbs" for s in deps],
        data=data,
        includes=includes,
        out_cpp=out_cpp,
        out_schema=out_schema,
        visibility=["//visibility:public"],
    )

    native.cc_library(
        name=name,
        srcs=out_cpp,
        data=out_schema,
        deps=["@flatbuffers"] + deps,
        includes=includes,
        **kargs)
