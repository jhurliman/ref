#pragma once

#include "NodeDefinition.h"

#include <json/json.h>
#include <ostream>
#include <third_party/ngraph.hpp>
#include <vector>

namespace ref {

class Graph {
public:
    Graph(const Json::Value& nodesArray);

    const std::vector<NodeDefinition>& nodes() const;
    const std::vector<NodeDefinition::Topic>& topics() const;
    const std::vector<NodeDefinition::TopicType>& types() const;
    void writeDot(std::ostream& stream) const;

private:
    void initialize();

    NGraph::sGraph _graph;
    std::vector<NodeDefinition> _nodes;
    std::vector<NodeDefinition::Topic> _topics;
    std::vector<NodeDefinition::TopicType> _types;
};

}  // namespace ref
