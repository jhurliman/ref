#include "node/Graph.h"

#include <core/Assert.h>
#include <core/Format.h>

namespace ref {

Graph::Graph(const Json::Value& nodesArray) {
    if (!nodesArray.isArray()) {
        throw std::runtime_error("Graph initialization requires a JSON array");
    }

    for (auto&& nodeJson : nodesArray) {
        auto res = NodeDefinition::Create(nodeJson);
        if (!res) {
            throw std::runtime_error(
                    Format("Invalid node definition:\n%s", nodeJson.toStyledString()));
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

    // First pass construction of the graph of nodes, topics, and the edges
    // between them. We need the full set of published (output) topics before
    // matching against triggers, which is why this is done in two passes
    for (const NodeDefinition& def : _nodes) {
        const std::string nodeVertex = "node:" + def.name();
        _graph.insert_vertex(nodeVertex);

        // This node -> output topics
        for (auto&& idAndTopic : def.outputs()) {
            const NodeDefinition::Topic& topic = idAndTopic.second;
            std::string topicVertex = "topic:" + topic.name;

            topicsMap[topic.name] = topic;
            typesMap[topic.type.name] = topic.type;

            _graph.insert_vertex(topicVertex);
            _graph.insert_edge(nodeVertex, topicVertex);
        }
    }

    // Convert topicsMap to a vector
    _topics.reserve(topicsMap.size());
    for (auto&& entry : topicsMap) {
        _topics.push_back(entry.second);
    }

    // Second pass, connecting matching input topics to each node. This must
    // happen after _topics has been fully populated
    for (NodeDefinition& def : _nodes) {
        const std::string nodeVertex = "node:" + def.name();

        // Trigger topics -> this node
        for (auto&& topicName : def.triggeringTopics(*this)) {
            std::string topicVertex = "topic:" + topicName;
            _graph.insert_edge(topicVertex, nodeVertex);
        }
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
