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
    using TickCallback = std::function<void(TestNode& node)>;

    TestNode(const NodeDefinition& def, const Graph& graph, TickCallback tickCallback)
            : NodeBase(def, graph)
            , _inputs(def.inputs())
            , _outputs(def.outputs())
            , _tickCallback(tickCallback) {}

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
    TickCallback _tickCallback;
};

void TestNode::tick() {
    _tickCallback(*this);
}

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

    auto tickCallback = [](TestNode& node) {
        std::cout << node.definition().name() << " ticked at time "
                  << Time::ToString(node.inputs()->currentTime()) << std::endl;
    };

    auto nodeCreator = [&](const NodeDefinition& def) -> std::unique_ptr<NodeBase> {
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
            return std::make_unique<TestNode>(def, g, tickCallback);
        default:
            throw std::runtime_error(Format("Unknown node type %s", def.nodeType()));
        }
    };

    Controller controller(g, nodeCreator);
    std::vector<NodeBase*> ready;

    auto currentTime = Time::FromNanoseconds(1);
    controller.readyNodes(currentTime, &ready);

    for (auto&& node : ready) {
        std::cout << node->definition().name() << std::endl;
    }

    controller.tickNodes(currentTime, ready);
}

}  // namespace ref
