#pragma once

#include "NodeDefinition.h"
#include "NodeMessages.h"

namespace ref {

class NodeBase {
public:
    const NodeDefinition& definition() const;

protected:
    NodeDefinition _definition;

    NodeBase(const NodeDefinition& def);
};

}  // namespace ref
