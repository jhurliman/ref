#include <core/Json.h>
#include <core/Strings.h>
#include <gtest/gtest.h>
#include <node/Graph.h>

namespace ref {

TEST(Graph, OneNode) {
    constexpr const char* SCHEMA_TYPE = "basic/BoolValue";
    constexpr const char* SCHEMA_HASH =
            "a66c7ec12e42a654ff9f38b5e432948449b20f6cc6ca088ae74bbdb0cca34d99";
    constexpr size_t SCHEMA_SIZE = 448;

    Json::Value json = ParseJSONFile("lib/node/test/data/one_node.jsonc");
    Graph g(".", json["nodes"]);

    ASSERT_EQ(1, g.nodes().size());

    const NodeDefinition& nodeDef = g.nodes()[0];
    EXPECT_EQ("a", nodeDef.name());
    EXPECT_EQ("A", nodeDef.nodeType());
    EXPECT_DOUBLE_EQ(1.0, nodeDef.triggers().timeout);
    EXPECT_EQ(size_t(0), nodeDef.triggers().topicMatches.size());
    EXPECT_EQ(size_t(0), nodeDef.triggeringTopics().size());
    EXPECT_EQ(size_t(0), nodeDef.inputs().size());

    ASSERT_EQ(1, nodeDef.outputs().size());
    const auto& [id, topicViaDef] = *(nodeDef.outputs().begin());
    EXPECT_EQ("x", id);
    EXPECT_EQ("/x", topicViaDef.name);
    EXPECT_EQ(SCHEMA_TYPE, topicViaDef.type.name);
    EXPECT_EQ(SCHEMA_HASH, topicViaDef.type.hash);
    EXPECT_EQ(SCHEMA_SIZE, topicViaDef.type.schema.size());

    ASSERT_EQ(1, g.topics().size());
    const NodeDefinition::Topic& topicViaList = g.topics()[0];
    EXPECT_EQ("/x", topicViaList.name);
    EXPECT_EQ(SCHEMA_TYPE, topicViaList.type.name);
    EXPECT_EQ(SCHEMA_HASH, topicViaList.type.hash);
    EXPECT_EQ(SCHEMA_SIZE, topicViaList.type.schema.size());

    ASSERT_EQ(1, g.types().size());
    const NodeDefinition::TopicType& type = g.types()[0];
    EXPECT_EQ(SCHEMA_TYPE, type.name);
    EXPECT_EQ(SCHEMA_HASH, type.hash);
    EXPECT_EQ(SCHEMA_SIZE, type.schema.size());

    std::stringstream ss;
    g.writeDot(ss);
    EXPECT_TRUE(StartsWith(ss.str(), "digraph G {"));
}

// TODO: Implement tests for more complex graphs when Graph actually does
// something

}  // namespace ref
