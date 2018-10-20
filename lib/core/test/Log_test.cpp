#include <core/Format.h>
#include <core/Log.h>

#include <gtest/gtest.h>

namespace ref {

TEST(Log, CharPtr) {
    LOG_DEBUG("LOG_DEBUG");
    LOG_INFO("LOG_INFO");
    LOG_WARN("LOG_WARN");
    LOG_ERROR("LOG_ERROR");
}

TEST(Log, String) {
    std::string debug = "LOG_DEBUG";
    std::string info = "LOG_INFO";
    std::string warn = "LOG_WARN";
    std::string error = "LOG_ERROR";

    LOG_DEBUG(debug);
    LOG_INFO(info);
    LOG_WARN(warn);
    LOG_ERROR(error);
}

TEST(Log, Format) {
    LOG_DEBUG("LOG_%s", "DEBUG");
    LOG_INFO("LOG_%s", "INFO");
    LOG_WARN("LOG_%s", "WARN");
    LOG_ERROR("LOG_%s", "ERROR");
}

TEST(Log, FormatFn) {
    LOG_DEBUG(Format("LOG_%s", "DEBUG"));
    LOG_INFO(Format("LOG_%s", "INFO"));
    LOG_WARN(Format("LOG_%s", "WARN"));
    LOG_ERROR(Format("LOG_%s", "ERROR"));
}

TEST(Log, Advanced) {
    std::string debug = "LOG_DEBUG";
    std::string info = "LOG_INFO";
    std::string warn = "LOG_WARN";
    std::string error = "LOG_ERROR";

    LOG_DEBUG(Format("LOG_%s", "%s").c_str(), debug);
    LOG_INFO(Format("LOG_%s", "%s").c_str(), info);
    LOG_WARN(Format("LOG_%s", "%s").c_str(), warn);
    LOG_ERROR(Format("LOG_%s", "%s").c_str(), error);
}

}  // namespace ref
