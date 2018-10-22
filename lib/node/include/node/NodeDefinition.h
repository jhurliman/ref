#pragma once

#include "Parameters.h"

#include <core/Optional.h>
#include <json/json.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ref {

class Graph;

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

    struct TriggerRequirements {
        std::vector<std::string> topicMatches;
        double timeout;
    };

    using InputOutputID = std::string;
    using TimeoutSec = double;
    using IDToTopicMap = std::unordered_map<InputOutputID, Topic>;
    using TopicList = std::vector<std::string>;

    static Optional<NodeDefinition> Create(const std::string& dataDir, const Json::Value& nodeJson);

    const std::string& name() const;
    const std::string& nodeType() const;
    const TriggerRequirements& triggers() const;
    const Parameters& parameters() const;
    const IDToTopicMap& inputs() const;
    const IDToTopicMap& outputs() const;

    const TopicList& triggeringTopics(const Graph& graph);

private:
    std::string _name;
    std::string _nodeType;
    TriggerRequirements _triggers;
    Parameters _parameters;
    IDToTopicMap _inputs;
    IDToTopicMap _outputs;
    std::unique_ptr<TopicList> _triggeringTopics;

    static Optional<Topic> parseInputOutput(const std::string& dataDir, const Json::Value& entry);

    NodeDefinition(
            const std::string& name,
            const std::string& nodeType,
            const TriggerRequirements& triggers,
            const Parameters& parameters,
            const IDToTopicMap& inputs,
            const IDToTopicMap& outputs);
};

}  // namespace ref
