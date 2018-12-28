#include "core/Assert.h"
#include "core/Json.h"

#include <fstream>
#include <sstream>

namespace ref {

Json::Value ParseJSON(const std::string& str) {
    Json::Value json;
    std::stringstream ss;
    ss << str;
    ss >> json;
    return json;
}

Json::Value ParseJSONFile(const std::string& filename) {
    Json::Value json;
    std::ifstream file(filename, std::ios::in);
    REF_ASSERT(file.is_open());
    file >> json;
    return json;
}

}
