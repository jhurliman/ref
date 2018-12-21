#pragma once

#include <cstdint>

namespace ref {

uint64_t constexpr Mix(char m, uint64_t s) {
    return ((s << 7) + ~(s >> 3)) + ~uint64_t(m);
}

uint64_t constexpr Hash(const char* m) {
    return (*m) ? Mix(*m, Hash(m + 1)) : 0;
}

}  // namespace ref
