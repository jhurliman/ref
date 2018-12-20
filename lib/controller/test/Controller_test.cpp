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

static void
Tick(Controller& controller, std::vector<NodeBase*>& ready, const Time::TimePoint currentTime) {
    ready.clear();
    controller.readyNodes(currentTime, &ready);
    controller.tickNodes(currentTime, ready);
}

static bool ContainsNode(const std::string& name, std::vector<NodeBase*>& nodes) {
    for (auto&& node : nodes) {
        if (node->definition().name() == name) {
            return true;
        }
    }
    return false;
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
        case Hash("Controls"):
        case Hash("ImageDecoding"):
        case Hash("ManeuverPlanning"):
        case Hash("MapTilePublisher"):
        case Hash("NavSatDriver"):
        case Hash("Odometry"):
        case Hash("Prediction"):
        case Hash("Recorder"):
        case Hash("Rectification"):
        case Hash("RoutePlanning"):
        case Hash("SystemHealth"):
        case Hash("Tracking"):
            return std::make_unique<TestNode>(def, g, tickCallback);
        default:
            throw std::runtime_error(Format("Unknown node type %s", def.nodeType()));
        }
    };

    Controller controller(g, nodeCreator);
    std::vector<NodeBase*> ready;

    Tick(controller, ready, Time::FromSeconds(0));
    EXPECT_EQ(size_t(0), ready.size());

    Tick(controller, ready, Time::FromSeconds(0.01));
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));

    Tick(controller, ready, Time::FromSeconds(0.05));
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));

    Tick(controller, ready, Time::FromSeconds(0.1));
    EXPECT_EQ(6, ready.size());
    EXPECT_TRUE(ContainsNode("camera_front", ready));
    EXPECT_TRUE(ContainsNode("camera_rear", ready));
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));
    EXPECT_TRUE(ContainsNode("prediction", ready));
    EXPECT_TRUE(ContainsNode("system_health", ready));
}

}  // namespace ref
