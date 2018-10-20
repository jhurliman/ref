#include "node/Graph.h"

#include <core/Assert.h>
#include <fstream>

namespace ref {

Graph::Graph(const Json::Value& nodesArray) {
    std::vector<NodeDefinition> nodeDefs;

    if (nodesArray.isArray()) {
        for (auto&& nodeJson : nodesArray) {
            const auto& def = NodeDefinition::Create(nodeJson);
            if (def) {
                nodeDefs.push_back(std::move(*def));
            }
        }
    }

    initialize(nodeDefs);
}

Graph::Graph(const std::vector<NodeDefinition>& nodeDefs) {
    initialize(nodeDefs);
}

void Graph::initialize(const std::vector<NodeDefinition>& nodeDefs) {
    for (auto&& def : nodeDefs) {
        _nodes.push_back(def);

        std::ifstream schema("lib/messages/definitions/recording/Header.bfbs", std::ifstream::binary);
        ASSERT_ALWAYS(schema.is_open());

        // Construct the graph of nodes, topics, and the edges between them

        const std::string nodeVertex = "node:" + def.name();
        _graph.insert_vertex(nodeVertex);

        for (auto&& idAndTopic : def.inputs()) {
            const NodeDefinition::Topic& topic = idAndTopic.second;
            const std::string topicVertex = "topic:" + topic.first;
            _graph.insert_vertex(topicVertex);
            _graph.insert_edge(topicVertex, nodeVertex);
        }

        for (auto&& idAndTopic : def.outputs()) {
            const NodeDefinition::Topic& topic = idAndTopic.second;
            const std::string topicVertex = "topic:" + topic.first;
            _graph.insert_vertex(topicVertex);
            _graph.insert_edge(nodeVertex, topicVertex);
        }
    }
}

const std::vector<NodeDefinition>& Graph::nodes() {
    return _nodes;
}

const std::vector<messages::recording::TopicDefinition>& Graph::topics() {
    return _topics;
}

const std::vector<messages::recording::TypeDefinition>& Graph::types() {
    return _types;
}

void Graph::writeDot(std::ostream& stream) {
    constexpr std::string_view prologue = "digraph G {\n";
    constexpr std::string_view epilogue = "}\n";
    using GraphIterator = NGraph::sGraph::const_iterator;

    stream.write(prologue.data(), prologue.size());

    // Print all the vertices first
    for (GraphIterator it = _graph.begin(); it != _graph.end(); it++) {
        auto& vertex = it->first;
        std::string line;
        std::string shortName = vertex.substr(vertex.find(':') + 1);

        if (vertex.find("node:") != std::string::npos) {
            line = "  " + shortName + " [shape=box];\n";
        } else {
            line = "  " + shortName + ";\n";
        }
        stream.write(line.c_str(), line.size());
    }
    stream.write("\n", 1);

    // Print the edges
    for (GraphIterator it = _graph.begin(); it != _graph.end(); it++) {
        auto& vertex = it->first;
        auto& insOuts = it->second;
        auto& outputs = insOuts.second;
        std::string shortName = vertex.substr(vertex.find(':') + 1);

        for (auto& outVertex : outputs) {
            std::string line = "  " + shortName + " -> " + outVertex + ";\n";
            stream.write(line.c_str(), line.size());
        }
    }
    stream.write(epilogue.data(), epilogue.size());
}

}  // namespace ref
