#include "node/NodeBase.h"

namespace ref {

NodeBase::NodeBase(const NodeDefinition& def, const Graph& graph)
        : _definition(def), _graph(graph) {}

const NodeDefinition& NodeBase::definition() const {
    return _definition;
}

const Graph& NodeBase::graph() const {
    return _graph;
}

std::mutex& NodeBase::mutex() const {
    return _mutex;
}

}  // namespace ref
