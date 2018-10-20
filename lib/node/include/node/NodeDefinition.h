#pragma once

#include "Parameters.h"

#include <core/Optional.h>
#include <json/json.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ref {

class NodeDefinition {
public:
    struct TopicType {
        std::string name;
        std::string hash;
        std::vector<uint8_t> schema;
    };

    struct Topic {
        std::string name;
        TopicType type;
    };

    using TopicName = std::string;
    using InputOutputID = std::string;
    using TimeoutSec = double;
    using TriggerRequirements = std::pair<std::vector<TopicName>, TimeoutSec>;
    using IDToTopicMap = std::unordered_map<InputOutputID, Topic>;

    static Optional<NodeDefinition> Create(const Json::Value& nodeJson);

    const std::string& name() const;
    const std::string& nodeType() const;
    const TriggerRequirements& triggers() const;
    const Parameters& parameters() const;
    const IDToTopicMap& inputs() const;
    const IDToTopicMap& outputs() const;

private:
    std::string _name;
    std::string _nodeType;
    TriggerRequirements _triggers;
    Parameters _parameters;
    IDToTopicMap _inputs;
    IDToTopicMap _outputs;

    static Optional<Topic> parseInputOutput(const Json::Value& entry);

    NodeDefinition(
            const std::string& name,
            const std::string& nodeType,
            const TriggerRequirements& triggers,
            const Parameters& parameters,
            const IDToTopicMap& inputs,
            const IDToTopicMap& outputs);
};

}  // namespace ref
