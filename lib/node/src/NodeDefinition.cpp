#include "node/NodeDefinition.h"
#include "node/Graph.h"

#include <core/FileUtils.h>
#include <core/Log.h>
#include <core/WildcardMatch.h>
#include <fstream>
#include <third_party/sha256.h>

namespace ref {

constexpr const char* BINARY_SCHEMA_PATH = "lib/messages/definitions/";
constexpr const char* BINARY_SCHEMA_EXT = ".bfbs";

// TopicType ///////////////////////////////////////////////////////////////////

NodeDefinition::TopicType::TopicType() {}

NodeDefinition::TopicType::TopicType(
        const std::string& name_,
        const std::string& hash_,
        const std::vector<uint8_t>& schema_)
        : name(name_), hash(hash_), schema(schema_) {}

// Topic ///////////////////////////////////////////////////////////////////////

NodeDefinition::Topic::Topic() {}

NodeDefinition::Topic::Topic(const std::string& name_, const TopicType& type_)
        : name(name_), type(type_) {}

// NodeDefinition //////////////////////////////////////////////////////////////

Result<NodeDefinition>
NodeDefinition::Create(const std::string& dataDir, const Json::Value& nodeJson) {
    std::string name = nodeJson["name"].asString();
    if (name.empty()) {
        return std::runtime_error("NodeDefinition is missing field 'name'");
    }

    std::string type = nodeJson["type"].asString();
    if (type.empty()) {
        return std::runtime_error("NodeDefinition is missing field 'type'");
    }

    TriggerRequirements reqs;
    auto& topicsJson = nodeJson["trigger"]["topics"];
    if (topicsJson.isArray()) {
        for (const auto& el : topicsJson) {
            reqs.topicMatches.push_back(el.asString());
        }
    }

    std::string condition = nodeJson["trigger"]["condition"].asString();
    if (condition.empty() || condition == "any") {
        reqs.condition = Condition::Any;
    } else if (condition == "all") {
        reqs.condition = Condition::All;
    } else if (condition == "interval") {
        reqs.condition = Condition::Interval;
    } else if (!condition.empty()) {
        return std::runtime_error(Format("Unrecognized condition '%s'", condition));
    }

    reqs.timeout = nodeJson["trigger"]["timeout"].asDouble();

    Parameters params(nodeJson["parameters"]);

    IDToTopicNameMap inputNames;
    const auto& inputsJson = nodeJson["inputs"];
    if (inputsJson.isObject()) {
        for (auto&& it = inputsJson.begin(); it != inputsJson.end(); ++it) {
            if (!(*it).isString()) {
                return std::runtime_error(Format("Invalid input '%s'", it.key().asString()));
            }

            inputNames[it.key().asString()] = (*it).asString();
        }
    }

    IDToTopicMap outputs;
    auto& outputsJson = nodeJson["outputs"];
    if (outputsJson.isObject()) {
        for (auto&& it = outputsJson.begin(); it != outputsJson.end(); ++it) {
            auto output = NodeDefinition::parseOutput(dataDir, *it);
            if (!output.isOk()) {
                return output.error();
            }

            const Topic topic = output.value();
            outputs[it.key().asString()] = topic;
        }
    }

    return NodeDefinition(name, type, reqs, params, inputNames, outputs);
}

NodeDefinition::Topic
NodeDefinition::LookupTopic(const NodeDefinition::IDToTopicMap& map, const char* id) {
    auto it = map.find(id);
    if (it != map.end()) {
        return it->second;
    }
    return {};
}

void NodeDefinition::init(const Graph& graph) {
    // Convert the map of IDs to input topic names into a map of IDs to input
    // topics
    for (auto&& [id, topicName] : _inputNames) {
        auto match = graph.getTopicByName(topicName);
        if (!match) {
            throw std::runtime_error(Format("Topic %s is not published in the graph", topicName));
        }
        _inputs[id] = *match;
    }

    // Compare this node's topic trigger patterns to all published topics in the
    // graph and construct the list of topics that will trigger this node
    _triggeringTopics.clear();
    for (const NodeDefinition::Topic& topic : graph.topics()) {
        for (const std::string& trigger : _triggers.topicMatches) {
            if (WildcardMatch(topic.name, trigger)) {
                _triggeringTopics.push_back(topic.name);
                break;
            }
        }
    }
}

const std::string& NodeDefinition::name() const {
    return _name;
}

const std::string& NodeDefinition::nodeType() const {
    return _nodeType;
}

const NodeDefinition::TriggerRequirements& NodeDefinition::triggers() const {
    return _triggers;
}

const Parameters& NodeDefinition::parameters() const {
    return _parameters;
}

const NodeDefinition::IDToTopicMap& NodeDefinition::inputs() const {
    return _inputs;
}

const NodeDefinition::IDToTopicMap& NodeDefinition::outputs() const {
    return _outputs;
}

const NodeDefinition::TopicList& NodeDefinition::triggeringTopics() const {
    return _triggeringTopics;
}

Result<NodeDefinition::Topic>
NodeDefinition::parseOutput(const std::string& dataDir, const Json::Value& entry) {
    if (!entry.isObject()) {
        return std::runtime_error("Output entry is not an object");
    }

    const auto& topicName = entry["topic"].asString();
    const auto& typeStr = entry["type"].asString();
    if (topicName.empty() || typeStr.empty()) {
        return std::runtime_error("Missing 'topic' or 'type' field(s)");
    }

    Topic topic;
    topic.name = topicName;
    topic.type.name = typeStr;

    // Load the binary schema for this type
    const std::string path = (dataDir.empty() ? std::string() : dataDir + "/") + BINARY_SCHEMA_PATH
            + typeStr + BINARY_SCHEMA_EXT;
    std::ifstream schema(path, std::ifstream::binary);
    if (!schema.is_open()) {
        return std::runtime_error(Format("Cannot open schema %s", path));
    }
    topic.type.schema.reserve(filesystem::FileLength(schema));
    std::copy(
            std::istream_iterator<uint8_t>(schema),
            std::istream_iterator<uint8_t>(),
            std::back_inserter(topic.type.schema));

    // Compute the SHA-256 hash of the binary schema
    SHA256 sha256;
    sha256(topic.type.schema.data(), topic.type.schema.size());
    topic.type.hash = sha256.getHash();

    return topic;
}

NodeDefinition::NodeDefinition(
        const std::string& name,
        const std::string& nodeType,
        const TriggerRequirements& triggers,
        const Parameters& parameters,
        const IDToTopicNameMap& inputNames,
        const IDToTopicMap& outputs)
        : _name(name)
        , _nodeType(nodeType)
        , _triggers(triggers)
        , _parameters(parameters)
        , _inputNames(inputNames)
        , _outputs(outputs) {}

}  // namespace ref
