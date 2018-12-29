#pragma once

#include "Result.h"

#include <json/json.h>

namespace ref {

Json::Value ParseJSON(const std::string& str);

Result<Json::Value> ParseJSONFile(const std::string& filename);

}
