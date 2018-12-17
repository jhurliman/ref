#include "node/NodeBase.h"

#include "node/NodeInputs.h"
#include "node/NodeOutputs.h"

namespace ref {

NodeBase::NodeBase(const NodeDefinition& def, const Graph& graph)
        : _definition(def), _graph(graph), _lastTick{} {}

const NodeDefinition& NodeBase::definition() const {
    return _definition;
}

const Graph& NodeBase::graph() const {
    return _graph;
}

std::mutex& NodeBase::mutex() const {
    return _mutex;
}

bool NodeBase::readyToTick(const Time::TimePoint currentTime) {
    // Check if all required topics are present
    if (_definition.triggeringTopics().size()) {
        bool hasAllTopics = true;
        const ReceivedMessageMap& received = inputs()->allMessages();
        for (auto&& topicName : _definition.triggeringTopics()) {
            auto it = received.find(topicName);
            if (it == received.end() || !it->second.data.size()) {
                hasAllTopics = false;
                break;
            }
        }

        if (hasAllTopics) {
            return true;
        }
    }

    // Check for a timeout
    const double timeout = _definition.triggers().timeout;
    if (timeout > 0.0 && Time::ToSeconds(currentTime - _lastTick) >= timeout) {
        return true;
    }

    return false;
}

void NodeBase::publishMessage(
        const Time::TimePoint currentTime,
        const PublishedMessageBase* message) {
    inputs()->copyFromOutput(message);
}

void NodeBase::executeTick(const Time::TimePoint currentTime) {
    _lastTick = currentTime;
    inputs()->setCurrentTime(currentTime);

    outputs()->clear();

    tick();
}

}  // namespace ref
