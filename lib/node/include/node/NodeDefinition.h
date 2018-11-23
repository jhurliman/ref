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

        TopicType();
        TopicType(const std::string& name_, const std::string& hash_, const std::vector<uint8_t>& schema_);
    };

    struct Topic {
        std::string name;
        TopicType type;

        Topic();
        Topic(const std::string& name_, const TopicType& type_);
    };

    struct TriggerRequirements {
        std::vector<std::string> topicMatches;
        double timeout;
    };

    using InputOutputID = std::string;
    using NodeName = std::string;
    using NodeType = std::string;
    using TopicName = std::string;
    using TimeoutSec = double;
    using IDToTopicMap = std::unordered_map<InputOutputID, Topic>;
    using IDToTopicNameMap = std::unordered_map<InputOutputID, TopicName>;
    using TopicList = std::vector<TopicName>;

    static Optional<NodeDefinition> Create(const std::string& dataDir, const Json::Value& nodeJson);

    static Topic LookupTopic(const IDToTopicMap& map, const char* id);

    void init(const Graph& graph);

    const std::string& name() const;
    const std::string& nodeType() const;
    const TriggerRequirements& triggers() const;
    const Parameters& parameters() const;
    const IDToTopicMap& inputs() const;
    const IDToTopicMap& outputs() const;
    const TopicList& triggeringTopics() const;

private:
    std::string _name;
    std::string _nodeType;
    TriggerRequirements _triggers;
    Parameters _parameters;
    IDToTopicNameMap _inputNames;
    IDToTopicMap _inputs;
    IDToTopicMap _outputs;
    TopicList _triggeringTopics;

    static Optional<Topic> parseOutput(const std::string& dataDir, const Json::Value& entry);

    NodeDefinition(
            const std::string& name,
            const std::string& nodeType,
            const TriggerRequirements& triggers,
            const Parameters& parameters,
            const IDToTopicNameMap& inputNames,
            const IDToTopicMap& outputs);
};

}  // namespace ref
