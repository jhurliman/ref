#include "core/FileUtils.h"

#include <third_party/path.h>

#ifdef __APPLE__
#include <errno.h>
#include <libproc.h>
#include <unistd.h>
#elif _WIN32
#else
#endif

namespace ref {

namespace filesystem {

size_t FileLength(std::ifstream& file) {
    auto pos = file.tellg();
    file.seekg(0, std::ios::end);
    size_t length = size_t(file.tellg());
    file.seekg(pos);
    return length;
}

std::string ApplicationDir() {
    return ::filesystem::path(ApplicationPath()).parent_path().str();
}

#ifdef __APPLE__

std::string ApplicationPath() {
    char path[PROC_PIDPATHINFO_MAXSIZE];
    pid_t pid = getpid();
    int ret = proc_pidpath(pid, path, sizeof(path));
    if (ret <= 0) {
        throw std::runtime_error(strerror(errno));
    }
    return std::string(path);
}

#elif _WIN32

std::string ApplicationPath() {
    return {};  // FIXME
}

#else

std::string ApplicationPath() {
    return {};  // FIXME
}

#endif

std::string JoinPath(const std::string& base, const std::string& part) {
    return (::filesystem::path(base) / ::filesystem::path(part)).str();
}

}  // namespace filesystem

}  // namespace ref
