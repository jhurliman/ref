#include "controller/Controller.h"

#include <core/Format.h>
#include <node/NodeInputs.h>
#include <unordered_set>

namespace ref {

Result<void> Controller::initialize(
        const Graph& g,
        const std::unordered_map<std::string, Controller::NodeFactoryFn>& nodeFactoryMap) {
    // Instantiate all of the nodes defined in the graph
    for (auto&& nodeDef : g.nodes()) {
        auto it = nodeFactoryMap.find(nodeDef.nodeType());
        if (it == nodeFactoryMap.end()) {
            // TODO: Get rid of wildcard matching. It's a hack to simplify testing
            it = nodeFactoryMap.find("*");
            if (it == nodeFactoryMap.end()) {
                return std::runtime_error(
                        Format("No factory function found for node type '%s'", nodeDef.nodeType()));
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

    return {};
}

void Controller::tick() {
    _readyCache.clear();

    ref::Time::TimePoint now = ref::Time::Now();
    ref::Time::TimePoint tickDeadline;

    readyNodes(now, &_readyCache, &tickDeadline);

    if (_readyCache.size()) {
        // One or more nodes are ready to tick, execute right away
        tickNodes(now, _readyCache);
    } else {
        // Find the delay in milliseconds between now and the next deadline
        double delayMS = ref::Time::DeltaMS(now, tickDeadline);
        // Clamp the delay to [0-1] milliseconds
        delayMS = std::max(0.0, std::min(1.0, delayMS));
        // Sleep for this amount of time
        ref::Time::Nanosleep(uint64_t(delayMS * 1e6));
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
