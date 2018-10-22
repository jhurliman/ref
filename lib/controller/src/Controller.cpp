#include "controller/Controller.h"

namespace ref {

Controller::Controller(const Graph& g) : _g(g) {}

void Controller::executeTicks(std::vector<NodeBase&>& nodes) {
    // for (auto&& node : nodes) {
    //     auto& outputs = node.definition().outputs();
    //     // FIXME: Graph needs to answer: give me a list of nodes given a list of input topics
    //     std::vector<NodeBase&> outputNodes;

    //     for (auto&& outputNode : outputNodes) {
    //         outputNode.
    //     }
    // }
}

}  // namespace ref
