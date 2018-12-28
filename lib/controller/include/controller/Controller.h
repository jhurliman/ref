#pragma once

#include <core/Time.h>
#include <functional>
#include <memory>
#include <node/Graph.h>
#include <node/NodeBase.h>
#include <node/NodeOutputs.h>
#include <unordered_map>
#include <vector>

#define NODE_FACTORY(nodeType) [](const ::ref::NodeDefinition& def, const ::ref::Graph g) { return std::make_unique<nodeType>(def, g); }

namespace ref {

class Controller {
public:
    using NodeFactoryFn = std::function<std::unique_ptr<NodeBase>(const NodeDefinition& nodeDef, const Graph& g)>;

    void initialize(const Graph& g, const std::unordered_map<std::string, NodeFactoryFn>& nodeFactoryMap);
    void readyNodes(Time::TimePoint currentTime, std::vector<NodeBase*>* ready, Time::TimePoint* tickDeadline);
    void tickNodes(Time::TimePoint currentTime, std::vector<NodeBase*>& nodes);
    void publishMessages(NodeOutputs& messages);

private:
    std::unordered_map<NodeDefinition::NodeName, std::unique_ptr<NodeBase>> _nodes;
    std::unordered_map<NodeDefinition::TopicName, std::vector<NodeBase*>> _topicsToNodes;
};

}  // namespace ref
