#pragma once

#include <iostream>
#include <third_party/tinyformat.h>

#define STR(a) #a

namespace ref {

template<typename... Args>
void Format(std::ostream& out, const char* fmt, const Args&... args) {
    tinyformat::vformat(out, fmt, tinyformat::makeFormatList(args...));
}

template<typename... Args>
std::string Format(const char* fmt, const Args&... args) {
    std::ostringstream out;
    Format(out, fmt, args...);
    return out.str();
}

}  // namespace ref
