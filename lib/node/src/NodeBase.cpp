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

bool NodeBase::readyToTick(const Time::TimePoint currentTime, Time::TimePoint* tickDeadline) {
    Time::TimePoint unused;
    if (!tickDeadline) {
        tickDeadline = &unused;
    }
    *tickDeadline = Time::DISTANT_FUTURE;

    auto& triggers = _definition.triggers();

    // Check if the input topics condition has been met
    switch (triggers.condition) {
    case NodeDefinition::Condition::Any:
        if (hasAnyInputs()) {
            *tickDeadline = currentTime;
            return true;
        }
        break;
    case NodeDefinition::Condition::All:
        if (hasAllInputs()) {
            *tickDeadline = currentTime;
            return true;
        }
        break;
    case NodeDefinition::Condition::Interval:
        break;
    }

    // Check for a timeout
    const double timeout = triggers.timeout;
    if (timeout > 0.0) {
        // Set the deadline to whenever this node times out in the future or the
        // current time, whichever is later
        *tickDeadline = Time::Max(Time::AddSeconds(_lastTick, timeout), currentTime);

        if (Time::ToSeconds(currentTime - _lastTick) >= timeout) {
            return true;
        }
    }

    return false;
}

void NodeBase::executeTick(const Time::TimePoint currentTime) {
    _lastTick = currentTime;
    inputs()->setCurrentTime(currentTime);

    outputs()->clear();

    tick();

    inputs()->clear();
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
