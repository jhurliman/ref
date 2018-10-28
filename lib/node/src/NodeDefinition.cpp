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

NodeDefinition::TopicType::TopicType() {}

NodeDefinition::TopicType::TopicType(
        const std::string& name_,
        const std::string& hash_,
        const std::vector<uint8_t>& schema_)
        : name(name_), hash(hash_), schema(schema_) {}

NodeDefinition::Topic::Topic() {}

NodeDefinition::Topic::Topic(const std::string& name_, const TopicType& type_)
        : name(name_), type(type_) {}

Optional<NodeDefinition>
NodeDefinition::Create(const std::string& dataDir, const Json::Value& nodeJson) {
    std::string name = nodeJson["name"].asString();
    if (name.empty()) {
        LOG_ERROR("NodeDefinition is missing field 'name'");
        return {};
    }

    std::string type = nodeJson["type"].asString();
    if (type.empty()) {
        LOG_ERROR("NodeDefinition is missing field 'type'");
        return {};
    }

    TriggerRequirements reqs;
    auto& topicsJson = nodeJson["trigger"]["topics"];
    if (topicsJson.isArray()) {
        for (const auto& el : topicsJson) {
            reqs.topicMatches.push_back(el.asString());
        }
    }
    reqs.timeout = nodeJson["trigger"]["timeout"].asDouble();

    Parameters params(nodeJson["parameters"]);

    IDToTopicMap inputs;
    const auto& inputsJson = nodeJson["inputs"];
    if (inputsJson.isObject()) {
        for (auto&& it = inputsJson.begin(); it != inputsJson.end(); ++it) {
            const auto& input = NodeDefinition::parseInputOutput(dataDir, *it);
            if (!input) {
                LOG_ERROR(Format("Invalid input '%s'", it.key().asString()));
                return {};
            }

            const Topic topic = *input;
            inputs[it.key().asString()] = topic;
        }
    }

    IDToTopicMap outputs;
    auto& outputsJson = nodeJson["outputs"];
    if (outputsJson.isObject()) {
        for (auto&& it = outputsJson.begin(); it != outputsJson.end(); ++it) {
            const auto& output = NodeDefinition::parseInputOutput(dataDir, *it);
            if (!output) {
                LOG_ERROR(Format("Invalid output '%s'", it.key().asString()));
                return {};
            }

            const Topic topic = *output;
            outputs[it.key().asString()] = topic;
        }
    }

    return NodeDefinition(name, type, reqs, params, inputs, outputs);
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

const NodeDefinition::TopicList& NodeDefinition::triggeringTopics(const Graph& graph) {
    // Check if the result has already been cached
    // NOTE: This assumes this method is only called with a single graph!
    if (_triggeringTopics) {
        return *_triggeringTopics;
    }

    std::unique_ptr<TopicList> topics = std::make_unique<TopicList>();

    for (const NodeDefinition::Topic& topic : graph.topics()) {
        for (const std::string& trigger : _triggers.topicMatches) {
            if (WildcardMatch(topic.name, trigger)) {
                (*topics).push_back(topic.name);
                break;
            }
        }
    }

    _triggeringTopics = std::move(topics);
    return *_triggeringTopics;
}

Optional<NodeDefinition::Topic>
NodeDefinition::parseInputOutput(const std::string& dataDir, const Json::Value& entry) {
    if (!entry.isObject()) {
        LOG_ERROR("Input/output entry is not an object");
        return {};
    }

    const auto& topicName = entry["topic"].asString();
    const auto& typeStr = entry["type"].asString();
    if (topicName.empty() || typeStr.empty()) {
        LOG_ERROR("Missing 'topic' or 'type' field(s)");
        return {};
    }

    Topic topic;
    topic.name = topicName;
    topic.type.name = typeStr;

    // Load the binary schema for this type
    const std::string path = (dataDir.empty() ? std::string() : dataDir + "/") + BINARY_SCHEMA_PATH
            + typeStr + BINARY_SCHEMA_EXT;
    std::ifstream schema(path, std::ifstream::binary);
    if (!schema.is_open()) {
        LOG_ERROR("Cannot open schema %s", path);
        return {};
    }
    topic.type.schema.reserve(FileLength(schema));
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
        const IDToTopicMap& inputs,
        const IDToTopicMap& outputs)
        : _name(name)
        , _nodeType(nodeType)
        , _triggers(triggers)
        , _parameters(parameters)
        , _inputs(inputs)
        , _outputs(outputs) {}

}  // namespace ref
