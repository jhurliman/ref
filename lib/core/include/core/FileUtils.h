#pragma once

#include <fstream>

namespace ref {

namespace filesystem {

size_t FileLength(std::ifstream& file);

std::string ApplicationDir();

std::string ApplicationPath();

std::string JoinPath(const std::string& base, const std::string& part);

}  // namespace filesystem

}  // namespace ref
