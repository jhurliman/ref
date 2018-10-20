#pragma once

#include <json/json.h>
#include <vector>

namespace ref {

class Parameters {
public:
    Parameters(const Json::Value& paramsJson);

    size_t size() const;
    bool readBool(const char* key, bool defaultValue = false) const;
    int32_t readInt(const char* key, int32_t defaultValue = 0) const;
    double readDouble(const char* key, double defaultValue = 0.0) const;
    const std::string readString(const char* key, const std::string& defaultValue = "") const;
    const std::vector<bool> readArrayBool(const char* key) const;
    const std::vector<int32_t> readArrayInt(const char* key) const;
    const std::vector<double> readArrayDouble(const char* key) const;
    const std::vector<std::string> readArrayString(const char* key) const;

private:
    Json::Value _root;
};

}  // namespace ref
