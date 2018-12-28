#include "core/Assert.h"

namespace ref {

void Abort(const std::string_view message) noexcept {
    if (IsStdoutATerminal()) {
        std::cerr << ANSI_RED << message << ANSI_RESET << std::endl;
    } else {
        std::cerr << message << std::endl;
    }

#if defined(__arm__) || defined(__arm64__)
    __builtin_trap();
#else
    asm volatile(
            "int $3\n\t"
            "nop");
#endif

    abort();
}

void Abort(int lineNumber, const char* filename, const std::string_view message) noexcept {
    std::ostringstream stream;
    Format(stream, "ABORT at %s:%d", filename, lineNumber);

    if (message.data() != nullptr && !message.empty()) {
        stream << " - " << message;
    }

    Abort(stream.str());
}

void Assert(
        const char* expression,
        int lineNumber,
        const char* filename,
        const std::string_view message) noexcept {
    std::ostringstream stream;
    Format(stream, "ASSERT(%s) failed at %s:%d", expression, filename, lineNumber);

    if (message.data() != nullptr && !message.empty()) {
        stream << " - " << message;
    }

    Abort(stream.str());
}

void Assert(const char* expression, int lineNumber, const char* filename) noexcept {
    Assert(expression, lineNumber, filename, nullptr);
}

}  // namespace ref
