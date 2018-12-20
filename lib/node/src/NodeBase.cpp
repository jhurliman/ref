#include "node/NodeBase.h"

#include "node/NodeInputs.h"
#include "node/NodeOutputs.h"

namespace ref {

NodeBase::NodeBase(const NodeDefinition& def, const Graph& graph)
        : _definition(def), _graph(graph), _lastTick(Time::FromNanoseconds(0)) {}

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
    auto& triggers = _definition.triggers();

    // Check if the input topics condition has been met
    switch (triggers.condition) {
    case NodeDefinition::Condition::Any:
        if (hasAnyInputs()) {
            return true;
        }
        break;
    case NodeDefinition::Condition::All:
        if (hasAllInputs()) {
            return true;
        }
        break;
    case NodeDefinition::Condition::Interval:
        break;
    }

    // Check for a timeout
    const double timeout = triggers.timeout;
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

bool NodeBase::hasAnyInputs() {
    const ReceivedMessageMap& received = inputs()->allMessages();
    for (auto&& topicName : _definition.triggeringTopics()) {
        auto it = received.find(topicName);
        if (it != received.end() && it->second.data.size()) {
            return true;
        }
    }

    return false;
}

bool NodeBase::hasAllInputs() {
    if (!_definition.triggeringTopics().size()) {
        return false;
    }

    const ReceivedMessageMap& received = inputs()->allMessages();
    for (auto&& topicName : _definition.triggeringTopics()) {
        auto it = received.find(topicName);
        if (it == received.end() || !it->second.data.size()) {
            return false;
        }
    }

    return true;
}

}  // namespace ref
