#pragma once

#include "Graph.h"
#include "NodeDefinition.h"
#include "PublishedMessage.h"

#include <core/Time.h>
#include <mutex>

namespace ref {

class NodeInputs;
class NodeOutputs;

class NodeBase {
public:
    virtual ~NodeBase() {};

    const NodeDefinition& definition() const;
    const Graph& graph() const;
    std::mutex& mutex() const;
    bool readyToTick(const Time::TimePoint currentTime);
    void publishMessage(const Time::TimePoint currentTime, const PublishedMessageBase* message);
    void executeTick(const Time::TimePoint currentTime);

    virtual NodeInputs* inputs() = 0;
    virtual NodeOutputs* outputs() = 0;

protected:
    NodeBase(const NodeDefinition& def, const Graph& graph);
    virtual void tick() = 0;

private:
    const NodeDefinition& _definition;
    const Graph& _graph;
    mutable std::mutex _mutex;
    Time::TimePoint _lastTick;

    bool hasAnyInputs();
    bool hasAllInputs();

    // Delete the copy/assignment constructors
    NodeBase(const NodeBase&) = delete;
    NodeBase& operator=(NodeBase const&) = delete;
};

}  // namespace ref
