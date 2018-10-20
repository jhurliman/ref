#include "node/Parameters.h"

namespace ref {

Parameters::Parameters(const Json::Value& paramsJson) : _root(paramsJson) {
    if (!_root.isObject()) {
        _root = Json::Value(Json::objectValue);
    }
}

size_t Parameters::size() const {
    return _root.size();
}

bool Parameters::readBool(const char* key, bool defaultValue) const {
    return _root.get(key, defaultValue).asBool();
}

int32_t Parameters::readInt(const char* key, int32_t defaultValue) const {
    return _root.get(key, defaultValue).asInt();
}

double Parameters::readDouble(const char* key, double defaultValue) const {
    return _root.get(key, defaultValue).asDouble();
}

const std::string Parameters::readString(const char* key, const std::string& defaultValue) const {
    return _root.get(key, defaultValue).asString();
}

const std::vector<bool> Parameters::readArrayBool(const char* key) const {
    auto& inArray = _root[key];
    if (inArray.isArray()) {
        std::vector<bool> outArray(inArray.size());
        for (size_t i = 0; i < inArray.size(); ++i) {
            outArray[i] = inArray[Json::Value::ArrayIndex(i)].asBool();
        }
        return outArray;
    }
    return {};
}

const std::vector<int32_t> Parameters::readArrayInt(const char* key) const {
    auto& inArray = _root[key];
    if (inArray.isArray()) {
        std::vector<int32_t> outArray(inArray.size());
        for (size_t i = 0; i < inArray.size(); ++i) {
            outArray[i] = inArray[Json::Value::ArrayIndex(i)].asInt();
        }
        return outArray;
    }
    return {};
}

const std::vector<double> Parameters::readArrayDouble(const char* key) const {
    auto& inArray = _root[key];
    if (inArray.isArray()) {
        std::vector<double> outArray(inArray.size());
        for (size_t i = 0; i < inArray.size(); ++i) {
            outArray[i] = inArray[Json::Value::ArrayIndex(i)].asDouble();
        }
        return outArray;
    }
    return {};
}

const std::vector<std::string> Parameters::readArrayString(const char* key) const {
    auto& inArray = _root[key];
    if (inArray.isArray()) {
        std::vector<std::string> outArray(inArray.size());
        for (size_t i = 0; i < inArray.size(); ++i) {
            outArray[i] = inArray[Json::Value::ArrayIndex(i)].asString();
        }
        return outArray;
    }
    return {};
}

}  // namespace ref
