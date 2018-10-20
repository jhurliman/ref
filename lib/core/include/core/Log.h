#pragma once

#include "Format.h"

#include <string>

namespace ref {

enum LogLevel { Debug = 0, Info, Warn, Error };

#define ANSI_WHITE "\033[1;37m"
#define ANSI_GREEN "\033[1;32m"
#define ANSI_YELLOW "\033[1;33m"
#define ANSI_RED "\033[1;31m"
#define ANSI_RESET "\033[0m"

bool IsStdoutATerminal();

void Log(LogLevel level, const std::string& str);

template<typename... Args>
inline void Log(LogLevel level, const char* fmt, const Args&... args) {
    Log(level, Format(fmt, args...));
}

inline void LOG_DEBUG(const std::string& str) {
    Log(LogLevel::Debug, str);
}

inline void LOG_INFO(const std::string& str) {
    Log(LogLevel::Info, str);
}

inline void LOG_WARN(const std::string& str) {
    Log(LogLevel::Warn, str);
}

inline void LOG_ERROR(const std::string& str) {
    Log(LogLevel::Error, str);
}

template<typename... Args>
inline void LOG_DEBUG(const char* fmt, const Args&... args) {
    Log(LogLevel::Debug, fmt, args...);
}

template<typename... Args>
inline void LOG_INFO(const char* fmt, const Args&... args) {
    Log(LogLevel::Info, fmt, args...);
}

template<typename... Args>
inline void LOG_WARN(const char* fmt, const Args&... args) {
    Log(LogLevel::Warn, fmt, args...);
}

template<typename... Args>
inline void LOG_ERROR(const char* fmt, const Args&... args) {
    Log(LogLevel::Error, fmt, args...);
}

}  // namespace ref
