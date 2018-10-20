#include "core/Log.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

namespace ref {

bool IsStdoutATerminal() {
    static bool isTTY = isatty(fileno(stdout));
    return isTTY;
}

void Log(LogLevel level, const std::string& str) {
    if (IsStdoutATerminal()) {
        switch (level) {
        case Debug: {
            std::cout << "[" ANSI_WHITE "DEBUG" ANSI_RESET "] " << str << std::endl;
        } break;
        case Info: {
            std::cout << "[" ANSI_GREEN "INFO" ANSI_RESET "] " << str << std::endl;
        } break;
        case Warn: {
            std::cerr << "[" ANSI_YELLOW "WARN" ANSI_RESET "] " << str << std::endl;
        } break;
        case Error: {
            std::cerr << ANSI_RED "[ERROR] " << str << ANSI_RESET << std::endl;
        } break;
        }
    } else {
        switch (level) {
        case Debug: {
            std::cout << "[DEBUG] " << str << std::endl;
        } break;
        case Info: {
            std::cout << "[INFO] " << str << std::endl;
        } break;
        case Warn: {
            std::cerr << "[WARN] " << str << std::endl;
        } break;
        case Error: {
            std::cerr << "[ERROR] " << str << std::endl;
        } break;
        }
    }
}

}  // namespace ref
