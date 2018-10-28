
REF_COPTS = [
    '-std=c++17',
    '-stdlib=libc++',

    '-flto=thin',

    '-Wall',
    '-Wextra',
    '-Weverything',

    # Promote to error
    '-Werror=return-type',

    # Disable
    '-Wno-c++98-compat',
    '-Wno-covered-switch-default',
    '-Wno-global-constructors',
    '-Wno-exit-time-destructors',
    '-Wno-padded',
    '-Wno-reserved-id-macro',
]
