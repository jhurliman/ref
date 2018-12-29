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

Result<Json::Value> ParseJSONFile(const std::string& filename) {
    Json::Value json;
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        return std::ios_base::failure(Format("Failed to open file %s", filename));
    }
    file >> json;
    return json;
}

}
