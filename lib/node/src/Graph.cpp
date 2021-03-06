#include "node/Graph.h"

#include <core/Assert.h>
#include <core/FileUtils.h>
#include <core/Format.h>

namespace ref {

Result<void> Graph::initialize(const std::string& dataDir, const Json::Value& nodesArray) {
    if (!nodesArray.isArray()) {
        return std::runtime_error("Graph initialization requires a JSON array");
    }

    for (auto&& nodeJson : nodesArray) {
        auto res = NodeDefinition::Create(dataDir, nodeJson);
        if (!res.isOk()) {
            return res.error();
        }

        auto& nodeDef = res.value();
        _nodes.push_back(std::move(nodeDef));
    }

    if (!_nodes.size()) {
        return std::runtime_error("Initialized an empty graph");
    }

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

    // Convert typesMap to a vector
    _types.reserve(typesMap.size());
    for (auto&& entry : typesMap) {
        _types.push_back(entry.second);
    }

    // Second pass, initializing each NodeDefinition and connecting published
    // topics to nodes via their declared triggering topics. This must happen
    // after _topics has been fully populated
    for (NodeDefinition& def : _nodes) {
        def.init(*this);

        // Trigger topics -> this node
        const std::string nodeVertex = "node:" + def.name();
        for (auto&& topicName : def.triggeringTopics()) {
            std::string topicVertex = "topic:" + topicName;
            _graph.insert_edge(topicVertex, nodeVertex);
        }
    }

    return {};
}

Result<void> Graph::initializeFromAppConfig(const std::string& appName) {
    using namespace ref::filesystem;

    std::string dataDir = JoinPath(ApplicationDir(), Format("%s.runfiles/ref_ws", appName));
    std::string configFile = JoinPath(dataDir, Format("%s/config/%s.jsonc", appName, appName));
    return initializeFromConfig(dataDir, configFile);
}

Result<void>
Graph::initializeFromConfig(const std::string& dataDir, const std::string& configFile) {
    auto res = ref::ParseJSONFile(configFile);
    if (!res.isOk()) {
        return res.error();
    }

    const Json::Value& nodes = res.value()["nodes"];
    if (!nodes.isArray()) {
        return std::runtime_error(Format("%s is missing required array 'nodes'", configFile));
    }

    return initialize(dataDir, nodes);
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

Optional<const NodeDefinition::Topic&> Graph::getTopicByName(const std::string& topicName) const {
    for (auto&& topic : _topics) {
        if (topic.name == topicName) {
            return topic;
        }
    }
    return {};
}

void Graph::writeDot(std::ostream& stream) const {
    constexpr std::string_view prologue = "digraph G {\n  rankdir=\"LR\";\n\n";
    constexpr std::string_view epilogue = "}\n";

    stream.write(prologue.data(), prologue.size());

    // Print all the vertices first
    for (auto&& it : _graph) {
        auto& vertex = it.first;
        std::string line;
        std::string shortName = vertex.substr(vertex.find(':') + 1);

        if (vertex.find("node:") != std::string::npos) {
            line = "  \"" + vertex + "\" [shape=doublecircle color=dodgerblue4];\n";
        } else {
            line = "  \"" + vertex + "\";\n";
        }
        stream.write(line.c_str(), std::streamsize(line.size()));
    }
    stream.write("\n", 1);

    // Print the edges
    for (auto&& it : _graph) {
        auto& vertex = it.first;
        auto& insOuts = it.second;
        auto& outputs = insOuts.second;

        for (auto& outVertex : outputs) {
            std::string line = "  \"" + vertex + "\" -> \"" + outVertex + "\";\n";
            stream.write(line.c_str(), std::streamsize(line.size()));
        }
    }
    stream.write(epilogue.data(), epilogue.size());
}

}  // namespace ref
