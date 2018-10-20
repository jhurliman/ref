#include "node/NodeBase.h"

namespace ref {

NodeBase::NodeBase(const NodeDefinition& def) : _definition(def) {}

const NodeDefinition& NodeBase::definition() const {
    return _definition;
}

}  // namespace ref
