#include <Header_generated.h>
#include <basic/BoolValueStamped_generated.h>
#include <core/Format.h>
#include <gtest/gtest.h>
#include <node/NodeOutputs.h>
#include <node/PublishedMessage.h>

namespace ref {

using namespace messages::basic;

class TestOutput : public NodeOutputs {
public:
    // Normally these would be private, but we're exposing them for unit testing
    PublishedMessage<BoolValueStampedT> _a;
    PublishedMessage<BoolValueStampedT> _b;

    inline std::unique_ptr<BoolValueStampedT>& a() {
        return _a.message;
    }

    inline std::unique_ptr<BoolValueStampedT>& b() {
        return _b.message;
    }

    TestOutput(NodeDefinition::IDToTopicMap& idToTopicMap)
            : NodeOutputs(idToTopicMap), _a(idToTopicMap[STR(a)]), _b(idToTopicMap[STR(b)]) {}

    virtual void serialize();
};

void TestOutput::serialize() {
    serializeMessage<BoolValueStamped, BoolValueStampedT>(_a);
    serializeMessage<BoolValueStamped, BoolValueStampedT>(_b);
}

////////////////////////////////////////////////////////////////////////////////

TEST(NodeOutputs, Empty) {
    NodeDefinition::IDToTopicMap idToTopicMap;
    TestOutput output(idToTopicMap);

    EXPECT_EQ(size_t(0), output._a.builder.GetSize());
    EXPECT_EQ(nullptr, output._a.message.get());
    EXPECT_EQ("", output._a.topic.name);
    EXPECT_EQ("", output._a.topic.type.hash);
    EXPECT_EQ("", output._a.topic.type.name);
    EXPECT_EQ(size_t(0), output._a.topic.type.schema.size());

    EXPECT_EQ(size_t(0), output._b.builder.GetSize());
    EXPECT_EQ(nullptr, output._b.message.get());
    EXPECT_EQ("", output._b.topic.name);
    EXPECT_EQ("", output._b.topic.type.hash);
    EXPECT_EQ("", output._b.topic.type.name);
    EXPECT_EQ(size_t(0), output._b.topic.type.schema.size());
}

TEST(NodeOutputs, SomeMissing) {
    NodeDefinition::TopicType typeA{"A", "x", {1, 2, 3}};
    NodeDefinition::Topic topicA{"/a", typeA};

    NodeDefinition::IDToTopicMap idToTopicMap{{"a", topicA}};
    TestOutput output(idToTopicMap);

    EXPECT_EQ(size_t(0), output._a.builder.GetSize());
    EXPECT_EQ(nullptr, output._a.message.get());
    EXPECT_EQ("/a", output._a.topic.name);
    EXPECT_EQ("x", output._a.topic.type.hash);
    EXPECT_EQ("A", output._a.topic.type.name);
    EXPECT_EQ(3, output._a.topic.type.schema.size());

    EXPECT_EQ(size_t(0), output._b.builder.GetSize());
    EXPECT_EQ(nullptr, output._b.message.get());
    EXPECT_EQ("", output._b.topic.name);
    EXPECT_EQ("", output._b.topic.type.hash);
    EXPECT_EQ("", output._b.topic.type.name);
    EXPECT_EQ(size_t(0), output._b.topic.type.schema.size());
}

TEST(NodeOutputs, Serialize) {
    NodeDefinition::TopicType typeA{"A", "x", {1, 2, 3}};
    NodeDefinition::Topic topicA{"/a", typeA};

    NodeDefinition::IDToTopicMap idToTopicMap{{"a", topicA}};
    TestOutput output(idToTopicMap);

    output.a() = std::make_unique<BoolValueStampedT>();
    output.a()->header = std::make_unique<messages::HeaderT>();
    output.a()->header->publish_stamp = 1;
    output.a()->header->sensor_stamp = 2;
    output.a()->value = true;

    EXPECT_EQ(size_t(0), output._a.builder.GetSize());
    EXPECT_NE(nullptr, output._a.message.get());

    output.serialize();

    EXPECT_EQ(56, output._a.builder.GetSize());
    EXPECT_NE(nullptr, output._a.message.get());

    EXPECT_EQ(size_t(0), output._b.builder.GetSize());
    EXPECT_EQ(nullptr, output._b.message.get());
}

}  // namespace ref
