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

static void
Tick(Controller& controller,
     std::vector<NodeBase*>& ready,
     const Time::TimePoint currentTime,
     Time::TimePoint& tickDeadline) {
    controller.readyNodes(currentTime, &ready, &tickDeadline);
    // std::cout << "[READY] ";
    // for (auto&& node : ready) {
    //     std::cout << node->definition().name() << ",";
    // }
    // std::cout << std::endl;
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

    // Print the graph to stdout in DOT (graphviz) format
    // std::stringstream ss;
    // g.writeDot(ss);
    // std::cout << ss.str() << std::endl;

    auto tickCallback = [](TestNode& node) {
        // std::cout << node.definition().name() << " ticked at time "
        //           << Time::ToString(node.inputs()->currentTime()) << std::endl;

        for (auto&& entry : node.definition().outputs()) {
            const auto& topic = entry.second;
            // std::cout << "Publishing to " << topic.name << std::endl;
            PublishedMessageBase* msg = GetOrCreateMessage(topic);
            msg->builder.PushFlatBuffer(reinterpret_cast<uint8_t*>(malloc(1)), 1);
            node.outputs()->allMessages()[topic.name] = msg;
        }
    };

    auto factory = [&](const NodeDefinition& def, const Graph& g_) -> std::unique_ptr<NodeBase> {
        return std::make_unique<TestNode>(def, g_, tickCallback);
    };

    Controller controller(g, factory);
    std::vector<NodeBase*> ready;
    Time::TimePoint t;
    Time::TimePoint tickDeadline;

    t = Time::FromNanoseconds(0);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(size_t(0), ready.size());
    EXPECT_EQ(Time::FromSeconds(0.01), tickDeadline);

    t = Time::FromNanoseconds(10000000);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_EQ(t, tickDeadline);

    t = Time::FromNanoseconds(50000000);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));
    EXPECT_EQ(t, tickDeadline);

    t = Time::FromNanoseconds(100000000);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(6, ready.size());
    EXPECT_TRUE(ContainsNode("camera_front", ready));
    EXPECT_TRUE(ContainsNode("camera_rear", ready));
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));
    EXPECT_TRUE(ContainsNode("prediction", ready));
    EXPECT_TRUE(ContainsNode("system_health", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("image_decoding", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("image_rect_front", ready));
    EXPECT_TRUE(ContainsNode("image_rect_rear", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("odometry", ready));
    EXPECT_TRUE(ContainsNode("tracking", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(3, ready.size());
    // Triggers because /predictions published in the third tick and /odom published in the previous
    // tick
    EXPECT_TRUE(ContainsNode("route_planning", ready));
    // Triggers because /odom published in the previous tick
    EXPECT_TRUE(ContainsNode("tracking", ready));
    EXPECT_TRUE(ContainsNode("map_tile_publisher", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(size_t(0), ready.size());
    EXPECT_EQ(Time::FromNanoseconds(110000000), tickDeadline);

    t = Time::FromNanoseconds(100000001);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(size_t(0), ready.size());
    EXPECT_EQ(Time::FromNanoseconds(110000000), tickDeadline);

    t = Time::FromNanoseconds(199999999);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(2, ready.size());
    EXPECT_TRUE(ContainsNode("controls", ready));
    EXPECT_TRUE(ContainsNode("maneuver_planning", ready));
    EXPECT_EQ(t, tickDeadline);

    t = Time::FromNanoseconds(200000000);
    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(4, ready.size());
    EXPECT_TRUE(ContainsNode("camera_front", ready));
    EXPECT_TRUE(ContainsNode("camera_rear", ready));
    EXPECT_TRUE(ContainsNode("prediction", ready));
    EXPECT_TRUE(ContainsNode("system_health", ready));
    EXPECT_EQ(t, tickDeadline);

    Tick(controller, ready, t, tickDeadline);
    EXPECT_EQ(1, ready.size());
    EXPECT_TRUE(ContainsNode("image_decoding", ready));
    EXPECT_EQ(t, tickDeadline);
}

}  // namespace ref
