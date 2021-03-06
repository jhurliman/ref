#pragma once

#include "Format.h"
#include "Log.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#if !defined(NDEBUG)
#define REF_DEBUG
#endif

#define REF_ABORT(...) ::ref::Abort(__LINE__, __FILE__, ##__VA_ARGS__)

#define REF_ASSERT(expression, ...) \
    do { \
        static_assert( \
                !__builtin_constant_p(expression), \
                #expression " is constant, use static_assert()"); \
        if (__builtin_expect(!(expression), 0)) { \
            ::ref::Assert(#expression, __LINE__, __FILE__, ##__VA_ARGS__); \
        } \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

namespace ref {

[[noreturn]] __attribute__((cold)) void Abort(const std::string_view message) noexcept;

[[noreturn]] __attribute__((cold)) void
Abort(int lineNumber, const char* filename, const std::string_view message) noexcept;

[[noreturn]] __attribute__((cold)) void
Assert(const char* expression,
       int lineNumber,
       const char* filename,
       const std::string_view message) noexcept;

[[noreturn]] __attribute__((cold)) void
Assert(const char* expression, int lineNumber, const char* filename) noexcept;

};  // namespace ref
