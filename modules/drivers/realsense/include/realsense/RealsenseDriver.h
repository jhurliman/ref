#pragma once

#include <node/NodeBase.h>

namespace ref {

class RealsenseDriver : NodeBase {
public:
    void tick(const NodeMessages& input, NodeMessages* output);
};

}  // namespace ref
