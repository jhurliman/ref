#pragma once

#include "NodeDefinition.h"

#include <core/Json.h>
#include <core/Optional.h>
#include <core/Result.h>
#include <ostream>
#include <third_party/ngraph.hpp>
#include <vector>

namespace ref {

class Graph {
public:
    Result<void> initialize(const std::string& dataDir, const Json::Value& nodesArray);
    Result<void> initializeFromAppConfig(const std::string& appName);
    Result<void> initializeFromConfig(const std::string& dataDir, const std::string& configFile);

    const std::vector<NodeDefinition>& nodes() const;
    const std::vector<NodeDefinition::Topic>& topics() const;
    const std::vector<NodeDefinition::TopicType>& types() const;

    Optional<const NodeDefinition::Topic&> getTopicByName(const std::string& topicName) const;

    void writeDot(std::ostream& stream) const;

private:
    NGraph::sGraph _graph;
    std::vector<NodeDefinition> _nodes;
    std::vector<NodeDefinition::Topic> _topics;
    std::vector<NodeDefinition::TopicType> _types;
};

}  // namespace ref
