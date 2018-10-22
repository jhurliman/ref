#include "core/FileUtils.h"

namespace ref {

size_t FileLength(std::ifstream& file) {
    auto pos = file.tellg();
    file.seekg(0, std::ios::end);
    size_t length = size_t(file.tellg());
    file.seekg(pos);
    return length;
}

}  // namespace ref
