#include "node/Graph.h"

#include <core/Assert.h>
#include <core/Format.h>
#include <fstream>

namespace ref {

Graph::Graph(const Json::Value& nodesArray) {
    if (!nodesArray.isArray()) {
        throw std::runtime_error("Graph initialization requires a JSON array");
    }

    for (auto&& nodeJson : nodesArray) {
        auto res = NodeDefinition::Create(nodeJson);
        if (!res) {
            throw std::runtime_error(
                    Format("Invalid node definition: %s", nodeJson.toStyledString()));
        }

        auto& nodeDef = *res;
        _nodes.push_back(std::move(nodeDef));
    }

    if (!_nodes.size()) {
        throw std::runtime_error("Initialized an empty graph");
    }

    initialize();
}

void Graph::initialize() {
    std::unordered_map<std::string, NodeDefinition::Topic> topicsMap;
    std::unordered_map<std::string, NodeDefinition::TopicType> typesMap;

    auto processIDsAndTopics = [&](const std::string nodeVertex,
                                   const NodeDefinition::IDToTopicMap& idsAndTopics) {
        for (auto&& idAndTopic : idsAndTopics) {
            const NodeDefinition::Topic& topic = idAndTopic.second;
            std::string topicVertex = "topic:" + topic.name;

            topicsMap[topic.name] = topic;
            typesMap[topic.type.name] = topic.type;

            _graph.insert_vertex(topicVertex);
            _graph.insert_edge(topicVertex, nodeVertex);
        }
    };

    // Construct the graph of nodes, topics, and the edges between them
    for (const NodeDefinition& def : _nodes) {
        const std::string nodeVertex = "node:" + def.name();
        _graph.insert_vertex(nodeVertex);

        processIDsAndTopics(nodeVertex, def.inputs());
        processIDsAndTopics(nodeVertex, def.outputs());
    }

    _topics.reserve(topicsMap.size());
    for (auto&& entry : topicsMap) {
        _topics.push_back(entry.second);
    }
}

const std::vector<NodeDefinition>& Graph::nodes() const {
    return _nodes;
}

const std::vector<NodeDefinition::Topic>& Graph::topics() const {
    return _topics;
}

const std::vector<NodeDefinition::TopicType>& Graph::types() const {
    return _types;
}

void Graph::writeDot(std::ostream& stream) const {
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
