#pragma once

#include <json/json.h>

namespace ref {

Json::Value ParseJSON(const std::string& str);

Json::Value ParseJSONFile(const std::string& filename);

}
