#include "node/NodeDefinition.h"

#include <core/Log.h>

namespace ref {

Optional<NodeDefinition> NodeDefinition::Create(const Json::Value& nodeJson) {
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
            reqs.first.push_back(el.asString());
        }
    }
    reqs.second = nodeJson["trigger"]["timeout"].asDouble();

    Parameters params(nodeJson["parameters"]);

    IDToTopicMap inputs;
    auto& inputsJson = nodeJson["inputs"];
    if (inputsJson.isObject()) {
        for (auto&& it = inputsJson.begin(); it != inputsJson.end(); ++it) {
            const std::string& id = it.key().asString();
            const auto& input = NodeDefinition::parseInputOutput(*it);
            if (!input) {
                LOG_ERROR(Format("Invalid input '%s'", id));
                return {};
            }

            inputs[id] = std::move(*input);
        }
    }

    IDToTopicMap outputs;
    auto& outputsJson = nodeJson["outputs"];
    if (outputsJson.isObject()) {
        for (auto&& it = outputsJson.begin(); it != outputsJson.end(); ++it) {
            const std::string& id = it.key().asString();
            const auto& output = NodeDefinition::parseInputOutput(*it);
            if (!output) {
                LOG_ERROR(Format("Invalid output '%s'", id));
                return {};
            }

            outputs[id] = std::move(*output);
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

Optional<NodeDefinition::Topic> NodeDefinition::parseInputOutput(const Json::Value& entry) {
    if (!entry.isObject()) {
        return {};
    }

    const auto& topicName = entry["topic"].asString();
    const auto& typeStr = entry["type"].asString();
    if (topicName.empty() || typeStr.empty()) {
        return {};
    }

    messages::recording::TypeDefinitionT type;
    type.name = typeStr;
    type.schema.resize(42);
    type.hash = "FIXMETOO";

    return std::make_pair(topicName, type);
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
