#include "controller/Controller.h"

#include <core/Format.h>
#include <node/NodeInputs.h>
#include <unordered_set>

namespace ref {

Controller::Controller(const Graph& g, Controller::NodeFactoryFn nodeCreator)
        : _g(g), _nodeCreator(nodeCreator) {
    // Instantiate all of the nodes defined in the graph
    for (auto&& nodeDef : _g.nodes()) {
        _nodes[nodeDef.name()] = _nodeCreator(nodeDef, _g);
    }

    // Create a mapping from topic name to nodes triggered by publishing that topic
    for (auto&& entry : _nodes) {
        NodeBase* nodePtr = entry.second.get();
        for (auto&& topicName : nodePtr->definition().triggeringTopics()) {
            _topicsToNodes[topicName].push_back(nodePtr);
        }
    }
}

void Controller::readyNodes(Time::TimePoint currentTime, std::vector<NodeBase*>* ready, Time::TimePoint* tickDeadline) {
    ready->clear();

    Time::TimePoint unused;
    if (!tickDeadline) {
        tickDeadline = &unused;
    }
    *tickDeadline = Time::DISTANT_FUTURE;

    Time::TimePoint thisDeadline;
    for (auto&& [name, node] : _nodes) {
        if (node->readyToTick(currentTime, &thisDeadline)) {
            *tickDeadline = currentTime;
            ready->push_back(node.get());
        } else {
            *tickDeadline = Time::Min(*tickDeadline, thisDeadline);
        }
    }
}

void Controller::tickNodes(Time::TimePoint currentTime, std::vector<NodeBase*>& nodes) {
    for (NodeBase* node : nodes) {
        // Tick this node then publish its outputs
        node->executeTick(currentTime);
        publishMessages(*node->outputs());
    }
}

void Controller::publishMessages(NodeOutputs& messages) {
    for (auto&& [topic, message] : messages.allMessages()) {
        auto it = _topicsToNodes.find(topic);
        if (it != _topicsToNodes.end()) {
            for (NodeBase* node : it->second) {
                node->inputs()->copyFromOutput(message);
            }
        }
    }
}

}  // namespace ref
