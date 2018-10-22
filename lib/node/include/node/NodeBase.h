#pragma once

#include "Graph.h"
#include "NodeDefinition.h"

namespace ref {

class NodeBase {
public:
    const NodeDefinition& definition() const;
    const Graph& graph() const;

protected:
    const NodeDefinition& _definition;
    const Graph& _graph;

    NodeBase(const NodeDefinition& def, const Graph& graph);
};

}  // namespace ref
