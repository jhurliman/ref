#pragma once

#include <node/Graph.h>
#include <node/NodeBase.h>
#include <vector>

namespace ref {

class Controller {
public:
    Controller(const Graph& g);

    std::vector<NodeBase&> readyNodes();

    // FIXME: Need the list of nodes to execute, then for each node retrieve the
    // nodes it publishes to and wire outputs to inputs after execution
    void executeTicks(std::vector<NodeBase&>& nodes);

private:
    const Graph& _g;
};

}  // namespace ref
