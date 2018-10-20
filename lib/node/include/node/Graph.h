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
    Graph(const std::vector<NodeDefinition>& nodeDefs);

    const std::vector<NodeDefinition>& nodes();
    const std::vector<NodeDefinition::Topic>& topics();
    const std::vector<NodeDefinition::TopicType>& types();
    void writeDot(std::ostream& stream);

private:
    void initialize(const std::vector<NodeDefinition>& nodeDefs);

    NGraph::sGraph _graph;
    std::vector<NodeDefinition> _nodes;
    std::vector<NodeDefinition::Topic> _topics;
    std::vector<NodeDefinition::TopicType> _types;
};

}  // namespace ref
