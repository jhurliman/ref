#pragma once

#include "Graph.h"
#include "NodeDefinition.h"

#include <mutex>

namespace ref {

class NodeBase {
public:
    const NodeDefinition& definition() const;
    const Graph& graph() const;
    std::mutex& mutex() const;

protected:
    NodeBase(const NodeDefinition& def, const Graph& graph);

private:
    const NodeDefinition& _definition;
    const Graph& _graph;
    mutable std::mutex _mutex;

    // Delete the copy/assignment constructors
    NodeBase(const NodeBase&) = delete;
    NodeBase& operator=(NodeBase const&) = delete;
};

}  // namespace ref
