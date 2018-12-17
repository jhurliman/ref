#include "controller/Controller.h"

#include <core/Format.h>
#include <node/NodeInputs.h>
#include <unordered_set>

namespace ref {

Controller::Controller(const Graph& g, Controller::NodeFactoryFn nodeCreator)
        : _g(g), _nodeCreator(nodeCreator) {
    // Instantiate all of the nodes defined in the graph
    for (auto&& nodeDef : _g.nodes()) {
        _nodes[nodeDef.name()] = _nodeCreator(nodeDef);
    }

    // Create a mapping from topic name to nodes triggered by publishing that topic
    for (auto&& entry : _nodes) {
        NodeBase* nodePtr = entry.second.get();
        for (auto&& topicName : nodePtr->definition().triggeringTopics()) {
            _topicsToNodes[topicName].push_back(nodePtr);
        }
    }
}

void Controller::readyNodes(Time::TimePoint currentTime, std::vector<NodeBase*>* ready) {
    for (auto&& [name, node] : _nodes) {
        if (node->readyToTick(currentTime)) {
            ready->push_back(node.get());
        }
    }
}

void Controller::tickNodes(Time::TimePoint currentTime, std::vector<NodeBase*>& nodes) {
    for (NodeBase* node : nodes) {
        // Tick this node, clear its inputs, then publish its outputs
        node->executeTick(currentTime);
        node->inputs()->clear();
        publishMessages(currentTime, *node->outputs());
    }
}

void Controller::publishMessages(const Time::TimePoint currentTime, NodeOutputs& messages) {
    for (auto&& [topic, message] : messages.allMessages()) {
        auto it = _topicsToNodes.find(topic);
        if (it != _topicsToNodes.end()) {
            for (NodeBase* node : it->second) {
                node->publishMessage(currentTime, message);
            }
        }
    }
}

}  // namespace ref
