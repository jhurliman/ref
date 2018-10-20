#pragma once

#include "NodeDefinition.h"

#include <json/json.h>
#include <ostream>
#include <recording/TopicDatabase_generated.h>
#include <recording/TypeDatabase_generated.h>
#include <third_party/ngraph.hpp>
#include <vector>

namespace ref {

class Graph {
public:
    Graph(const Json::Value& nodesArray);
    Graph(const std::vector<NodeDefinition>& nodeDefs);

    const std::vector<NodeDefinition>& nodes();
    const std::vector<messages::recording::TopicDefinition>& topics();
    const std::vector<messages::recording::TypeDefinition>& types();
    void writeDot(std::ostream& stream);

private:
    void initialize(const std::vector<NodeDefinition>& nodeDefs);

    NGraph::sGraph _graph;
    std::vector<NodeDefinition> _nodes;
    std::vector<messages::recording::TopicDefinition> _topics;
    std::vector<messages::recording::TypeDefinition> _types;
};

}  // namespace ref
