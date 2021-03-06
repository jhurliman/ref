#include <Header_generated.h>
#include <basic/BoolValueStamped_generated.h>
#include <core/Format.h>
#include <gtest/gtest.h>
#include <node/NodeInputs.h>

namespace ref {

using namespace messages::basic;

class TestInput : public NodeInputs {
public:
    std::unique_ptr<BoolValueStampedT> a;
    std::unique_ptr<BoolValueStampedT> b;

    TestInput(const NodeDefinition::IDToTopicMap& idToTopicMap) : NodeInputs(idToTopicMap) {}

    virtual void deserialize();
};

void TestInput::deserialize() {
    deserializeMessage<BoolValueStamped, BoolValueStampedT>(STR(a), &a);
    deserializeMessage<BoolValueStamped, BoolValueStampedT>(STR(b), &b);
}

////////////////////////////////////////////////////////////////////////////////

TEST(NodeInputs, Empty) {
    NodeDefinition::IDToTopicMap idToTopicMap;
    TestInput input(idToTopicMap);
    input.setCurrentTime(Time::FromNanoseconds(1));

    EXPECT_EQ(Time::FromNanoseconds(1), input.currentTime());
    EXPECT_EQ(size_t(0), input.allMessages().size());
    EXPECT_EQ(nullptr, input.a.get());
    EXPECT_EQ(nullptr, input.b.get());
}

TEST(NodeInputs, SomeMissing) {
    NodeDefinition::TopicType typeA{"A", "x", {1, 2, 3}};
    NodeDefinition::Topic topicA{"/a", typeA};

    NodeDefinition::IDToTopicMap idToTopicMap{{"a", topicA}};
    TestInput input(idToTopicMap);

    EXPECT_EQ(1, input.allMessages().size());
    EXPECT_EQ(nullptr, input.b.get());
    EXPECT_EQ(nullptr, input["/b"]);

    ASSERT_NE(nullptr, input["/a"]);
    EXPECT_EQ(size_t(0), input["/a"]->data.size());
    EXPECT_EQ(nullptr, input.a.get());
    EXPECT_EQ("/a", input["/a"]->topic.name);
    EXPECT_EQ("x", input["/a"]->topic.type.hash);
    EXPECT_EQ("A", input["/a"]->topic.type.name);
    EXPECT_EQ(3, input["/a"]->topic.type.schema.size());
}

TEST(NodeOutputs, CopyFromOutputsAndReset) {
    NodeDefinition::TopicType typeA{"A", "x", {1, 2, 3}};
    NodeDefinition::Topic topicA{"/a", typeA};

    NodeDefinition::IDToTopicMap idToTopicMap{{"a", topicA}};
    TestInput input(idToTopicMap);

    PublishedMessage<BoolValueStampedT> outputA(topicA);
    outputA.message = std::make_unique<BoolValueStampedT>();
    outputA.message->header = std::make_unique<messages::HeaderT>();
    outputA.message->header->frame_id = "frame_id";
    outputA.message->header->publish_stamp = 1;
    outputA.message->header->sensor_stamp = 2;
    outputA.message->value = true;
    outputA.builder.Finish(BoolValueStamped::Pack(outputA.builder, outputA.message.get()));

    PublishedMessageMap outputMap{{"/a", &outputA}};

    ASSERT_NE(nullptr, input["/a"]);
    EXPECT_EQ(size_t(0), input["/a"]->data.size());
    EXPECT_EQ(nullptr, input.a.get());
    EXPECT_EQ(nullptr, input.b.get());

    input.copyFromOutputs(outputMap);

    ASSERT_NE(nullptr, input["/a"]);
    EXPECT_EQ(80, input["/a"]->data.size());
    ASSERT_EQ(nullptr, input.a.get());
    EXPECT_EQ(nullptr, input.b.get());

    input.deserialize();

    ASSERT_NE(nullptr, input["/a"]);
    EXPECT_EQ(80, input["/a"]->data.size());
    ASSERT_NE(nullptr, input.a.get());
    ASSERT_NE(nullptr, input.a->header.get());
    EXPECT_EQ("frame_id", input.a->header->frame_id);
    EXPECT_EQ(1, input.a->header->publish_stamp);
    EXPECT_EQ(2, input.a->header->sensor_stamp);
    EXPECT_EQ(true, input.a->value);
}

}  // namespace ref
