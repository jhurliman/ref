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
    std::cout << "[READY] ";
    for (auto&& node : ready) {
        std::cout << node->definition().name() << ",";
    }
    std::cout << std::endl;
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

static PublishedMessageBase* GetOrCreateMessage(const NodeDefinition::Topic& topic) {
    static std::unordered_map<std::string, PublishedMessageBase> messages;

    auto it = messages.find(topic.name);
    if (it != messages.end()) {
        return &it->second;
    }

    auto res = messages.emplace(topic.name, topic);
    return &res.first->second;
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

        for (auto&& entry : node.definition().outputs()) {
            const auto& topic = entry.second;
            std::cout << "Publishing to " << topic.name << std::endl;
            PublishedMessageBase* msg = GetOrCreateMessage(topic);
            msg->builder.PushFlatBuffer(reinterpret_cast<uint8_t*>(malloc(1)), 1);
            node.outputs()->allMessages()[topic.name] = msg;
        }
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

    Tick(controller, ready, Time::FromNanoseconds(0));
    EXPECT_EQ(size_t(0), ready.size());

    Tick(controller, ready, Time::FromNanoseconds(10000000));
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));

    Tick(controller, ready, Time::FromNanoseconds(50000000));
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(6, ready.size());
    EXPECT_TRUE(ContainsNode("camera_front", ready));
    EXPECT_TRUE(ContainsNode("camera_rear", ready));
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));
    EXPECT_TRUE(ContainsNode("prediction", ready));
    EXPECT_TRUE(ContainsNode("system_health", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("image_decoding", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("image_rect_front", ready));
    EXPECT_TRUE(ContainsNode("image_rect_rear", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("odometry", ready));
    EXPECT_TRUE(ContainsNode("tracking", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(3, ready.size());
    // Triggers because /predictions published in the third tick and /odom published in the previous tick
    EXPECT_TRUE(ContainsNode("route_planning", ready));
    // Triggers because /odom published in the previous tick
    EXPECT_TRUE(ContainsNode("tracking", ready));
    EXPECT_TRUE(ContainsNode("map_tile_publisher", ready));

    Tick(controller, ready, Time::FromNanoseconds(100000000));
    EXPECT_EQ(size_t(0), ready.size());

    Tick(controller, ready, Time::FromNanoseconds(100000001));
    EXPECT_EQ(size_t(0), ready.size());

    Tick(controller, ready, Time::FromNanoseconds(199999999));
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));

    Tick(controller, ready, Time::FromNanoseconds(200000000));
    EXPECT_EQ(4, ready.size());
    EXPECT_TRUE(ContainsNode("camera_front", ready));
    EXPECT_TRUE(ContainsNode("camera_rear", ready));
    EXPECT_TRUE(ContainsNode("prediction", ready));
    EXPECT_TRUE(ContainsNode("system_health", ready));

    Tick(controller, ready, Time::FromNanoseconds(200000000));
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("image_decoding", ready));
}

}  // namespace ref
