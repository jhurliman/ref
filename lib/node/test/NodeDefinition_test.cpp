#include <core/Json.h>
#include <gtest/gtest.h>
#include <node/Graph.h>
#include <node/NodeDefinition.h>
#include <unordered_set>

namespace ref {

TEST(NodeDefinition, OneNode) {
    auto jsonRes = ParseJSONFile("lib/node/test/data/one_node.jsonc");
    ASSERT_TRUE(jsonRes.isOk());
    auto nodeRes = NodeDefinition::Create(".", jsonRes.value()["nodes"][0]);
    ASSERT_TRUE(nodeRes.isOk());

    const NodeDefinition& def = nodeRes.value();
    EXPECT_EQ("a", def.name());
    EXPECT_EQ("A", def.nodeType());
    EXPECT_EQ(size_t(0), def.triggers().topicMatches.size());
    EXPECT_EQ(NodeDefinition::Condition::Interval, def.triggers().condition);
    EXPECT_EQ(1.0, def.triggers().timeout);
    EXPECT_EQ(size_t(0), def.parameters().size());
    EXPECT_EQ(size_t(0), def.inputs().size());
    ASSERT_EQ(1, def.outputs().size());
    EXPECT_EQ("x", def.outputs().begin()->first);

    auto& topic = def.outputs().begin()->second;
    EXPECT_EQ("/x", topic.name);
    EXPECT_EQ("basic/BoolValueStamped", topic.type.name);
    EXPECT_EQ("b2a1382b9efb76f895b29a6085f9c96be2933583aa2c14e36d6b39ec92715873", topic.type.hash);
    EXPECT_EQ(452, topic.type.schema.size());
}

TEST(NodeDefinition, WildcardNode) {
    auto jsonRes = ParseJSONFile("lib/node/test/data/wildcard_graph.jsonc");
    ASSERT_TRUE(jsonRes.isOk());
    auto nodeRes = NodeDefinition::Create(".", jsonRes.value()["nodes"][0]);
    ASSERT_TRUE(nodeRes.isOk());

    NodeDefinition& def = nodeRes.value();
    EXPECT_EQ("w", def.name());
    EXPECT_EQ("W", def.nodeType());
    ASSERT_EQ(1, def.triggers().topicMatches.size());
    EXPECT_EQ("*", def.triggers().topicMatches[0]);
    EXPECT_EQ(NodeDefinition::Condition::Any, def.triggers().condition);
    EXPECT_EQ(0.0, def.triggers().timeout);
    EXPECT_EQ(size_t(0), def.parameters().size());
    EXPECT_EQ(size_t(0), def.inputs().size());
    ASSERT_EQ(size_t(0), def.outputs().size());

    Graph g;
    ASSERT_TRUE(g.initialize(".", jsonRes.value()["nodes"]).isOk());

    EXPECT_EQ(3, g.nodes().size());
    EXPECT_EQ(2, g.topics().size());

    EXPECT_EQ(size_t(0), def.triggeringTopics().size());
    def.init(g);
    const auto& topics = def.triggeringTopics();
    ASSERT_EQ(2, topics.size());

    std::unordered_set<std::string> expected = {"/x", "/y"};
    std::unordered_set<std::string> actual(topics.begin(), topics.end());
    EXPECT_EQ(expected, actual);
}

}  // namespace ref
