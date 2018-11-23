#pragma once

#include <core/Time.h>
#include <functional>
#include <memory>
#include <node/Graph.h>
#include <node/NodeBase.h>
#include <node/NodeOutputs.h>
#include <unordered_map>
#include <vector>

namespace ref {

class Controller {
public:
    using NodeFactoryFn = std::function<std::unique_ptr<NodeBase>(const NodeDefinition& nodeDef)>;

    Controller(const Graph& g, NodeFactoryFn nodeCreator);

    Time::HiResTimePoint nextDeadline();
    void readyNodes(Time::TimePoint currentTime, std::vector<NodeBase*>* ready);
    void publishMessages(const Time::TimePoint currentTime, NodeOutputs& messages);

private:
    const Graph& _g;
    const NodeFactoryFn _nodeCreator;
    std::unordered_map<NodeDefinition::NodeName, std::unique_ptr<NodeBase>> _nodes;
    std::unordered_map<NodeDefinition::TopicName, std::vector<NodeBase*>> _topicsToNodes;
};

}  // namespace ref
