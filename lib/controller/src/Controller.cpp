#include "controller/Controller.h"

#include <core/Format.h>
#include <node/NodeInputs.h>
#include <unordered_set>

namespace ref {

void Controller::initialize(
        const Graph& g,
        const std::unordered_map<std::string, Controller::NodeFactoryFn>& nodeFactoryMap) {
    // Instantiate all of the nodes defined in the graph
    for (auto&& nodeDef : g.nodes()) {
        const std::string& typeName = nodeDef.nodeType();
        auto it = nodeFactoryMap.find(typeName);
        if (it == nodeFactoryMap.end()) {
            it = nodeFactoryMap.find("*");
            if (it == nodeFactoryMap.end()) {
                REF_ABORT(Format("No factory function found for node type '%s'", typeName));
            }
        }
        _nodes[nodeDef.name()] = it->second(nodeDef, g);
    }

    // Create a mapping from topic name to nodes triggered by publishing that topic
    for (auto&& entry : _nodes) {
        NodeBase* nodePtr = entry.second.get();
        for (auto&& topicName : nodePtr->definition().triggeringTopics()) {
            _topicsToNodes[topicName].push_back(nodePtr);
        }
    }
}

void Controller::readyNodes(
        Time::TimePoint currentTime,
        std::vector<NodeBase*>* ready,
        Time::TimePoint* tickDeadline) {
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
