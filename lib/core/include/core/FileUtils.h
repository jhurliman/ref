#pragma once

#include <fstream>

namespace ref {

size_t FileLength(std::ifstream& file);

std::string ApplicationDir();

std::string ApplicationPath();

std::string JoinPath(const std::string& base, const std::string& part);

}  // namespace ref
