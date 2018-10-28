#include "JsonParse.h"

#include <gtest/gtest.h>
#include <node/Graph.h>
#include <node/NodeDefinition.h>
#include <set>

namespace ref {

TEST(NodeDefinition, OneNode) {
    Json::Value json = ParseJSONFile("lib/node/test/data/one_node.jsonc");
    Optional<NodeDefinition> defPtr = NodeDefinition::Create(".", json["nodes"][0]);
    ASSERT_TRUE(defPtr.has_value());

    const NodeDefinition& def = *defPtr;
    EXPECT_EQ("a", def.name());
    EXPECT_EQ("A", def.nodeType());
    EXPECT_EQ(size_t(0), def.triggers().topicMatches.size());
    EXPECT_EQ(1.0, def.triggers().timeout);
    EXPECT_EQ(size_t(0), def.parameters().size());
    EXPECT_EQ(size_t(0), def.inputs().size());
    ASSERT_EQ(1, def.outputs().size());
    EXPECT_EQ("x", def.outputs().begin()->first);

    auto& topic = def.outputs().begin()->second;
    EXPECT_EQ("/x", topic.name);
    EXPECT_EQ("basic/BoolValue", topic.type.name);
    EXPECT_EQ("a66c7ec12e42a654ff9f38b5e432948449b20f6cc6ca088ae74bbdb0cca34d99", topic.type.hash);
    EXPECT_EQ(448, topic.type.schema.size());
}

TEST(NodeDefinition, WildcardNode) {
    Json::Value json = ParseJSONFile("lib/node/test/data/wildcard_graph.jsonc");
    Optional<NodeDefinition> defPtr = NodeDefinition::Create(".", json["nodes"][0]);
    ASSERT_TRUE(defPtr.has_value());

    NodeDefinition& def = *defPtr;
    EXPECT_EQ("w", def.name());
    EXPECT_EQ("W", def.nodeType());
    ASSERT_EQ(1, def.triggers().topicMatches.size());
    EXPECT_EQ("*", def.triggers().topicMatches[0]);
    EXPECT_EQ(0.0, def.triggers().timeout);
    EXPECT_EQ(size_t(0), def.parameters().size());
    EXPECT_EQ(size_t(0), def.inputs().size());
    ASSERT_EQ(size_t(0), def.outputs().size());

    const Graph g(".", json["nodes"]);

    EXPECT_EQ(3, g.nodes().size());
    EXPECT_EQ(2, g.topics().size());
    ASSERT_EQ(def.triggeringTopics(g), def.triggeringTopics(g));
    const auto& topics = def.triggeringTopics(g);
    ASSERT_EQ(2, topics.size());

    std::set<std::string> expected = {"/x", "/y"};
    std::set<std::string> actual(topics.begin(), topics.end());
    EXPECT_EQ(expected, actual);
}

}  // namespace ref
