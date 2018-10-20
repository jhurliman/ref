#include "Format.h"
#include "Log.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define ABORT_ALWAYS(...) ::ref::Abort(__LINE__, __FILE__, ##__VA_ARGS__)

#define ASSERT_ALWAYS(expression, ...) \
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

[[noreturn]] __attribute__((cold)) void Abort(const char* message) noexcept;

[[noreturn]] __attribute__((cold)) void
Abort(int lineNumber, const char* filename, const char* message) noexcept;

__attribute__((cold)) void
Assert(const char* expression,
       int lineNumber,
       const char* filename,
       const std::string_view message) noexcept;

__attribute__((cold)) void
Assert(const char* expression, int lineNumber, const char* filename) noexcept;

};  // namespace ref
