#include <controller/Controller.h>
#include <core/Format.h>
#include <core/Json.h>
#include <gtest/gtest.h>
#include <node/NodeBase.h>
#include <node/NodeInputs.h>
#include <node/NodeOutputs.h>

namespace ref {

class TestNode : public NodeBase {
public:
    TestNode(const NodeDefinition& def, const Graph& graph)
            : NodeBase(def, graph), _inputs(def.inputs()), _outputs(def.outputs()) {}

    NodeInputs* inputs() override {
        return &_inputs;
    }

    NodeOutputs* outputs() override {
        return &_outputs;
    }

    void tick() override;

protected:
    NodeInputs _inputs;
    NodeOutputs _outputs;
};

void TestNode::tick() {}

uint64_t constexpr Mix(char m, uint64_t s) {
    return ((s << 7) + ~(s >> 3)) + ~uint64_t(m);
}

uint64_t constexpr Hash(const char* m) {
    return (*m) ? Mix(*m, Hash(m + 1)) : 0;
}

TEST(Controller, BasicRobot) {
    Json::Value json = ParseJSONFile("lib/controller/test/data/basic_robot.jsonc");
    Graph g(".", json["nodes"]);

#if 0
    // Print the graph to stdout in DOT (graphviz) format
    std::stringstream ss;
    g.writeDot(ss);
    std::cout << ss.str() << std::endl;
#endif

    auto nodeCreator = [g](const NodeDefinition& def) -> std::unique_ptr<NodeBase> {
        switch (Hash(def.nodeType().c_str())) {
        case Hash("CameraDriver"):
        case Hash("NavSatDriver"):
        case Hash("ImageDecoding"):
        case Hash("Rectification"):
        case Hash("Odometry"):
        case Hash("Tracking"):
        case Hash("RoutePlanning"):
        case Hash("ManeuverPlanning"):
        case Hash("Controls"):
        case Hash("Recorder"):
        case Hash("SystemHealth"):
            return std::make_unique<TestNode>(def, g);
        default:
            throw std::runtime_error(Format("Unknown node type %s", def.nodeType()));
        }
    };

    Controller controller(g, nodeCreator);

    constexpr size_t TOTAL_TICKS = 5;
    size_t ticks = 0;
    std::vector<NodeBase*> ready;

    do {
        auto currentTime = Time::FromNanoseconds(ticks);

        // Fetch the list of nodes that are ready to tick
        ready.clear();
        controller.readyNodes(currentTime, &ready);

        for (NodeBase* node : ready) {
            node->inputs()->setCurrentTime(currentTime);
            NodeOutputs* outputs = node->outputs();
            outputs->clear();

            // Tick this node and publish its outputs
            node->tick();
            controller.publishMessages(currentTime, *outputs);
        }
    } while (++ticks < TOTAL_TICKS);
}

}  // namespace ref
