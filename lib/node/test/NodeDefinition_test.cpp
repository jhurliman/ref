#include "JsonParse.h"

#include <gtest/gtest.h>
#include <node/NodeDefinition.h>

namespace ref {

TEST(NodeDefinition, OneNode) {
    Json::Value json = ParseJSONFile("lib/node/test/data/one_node.jsonc");
    Optional<NodeDefinition> defPtr = NodeDefinition::Create(json["nodes"][0]);
    ASSERT_TRUE(defPtr.has_value());

    const NodeDefinition& def = *defPtr;
    EXPECT_EQ("a", def.name());
    EXPECT_EQ("A", def.nodeType());
    EXPECT_EQ(0, def.triggers().first.size());
    EXPECT_EQ(1.0, def.triggers().second);
    EXPECT_EQ(0, def.parameters().size());
    EXPECT_EQ(0, def.inputs().size());
    ASSERT_EQ(1, def.outputs().size());
    EXPECT_EQ("x", def.outputs().begin()->first);

    auto& topic = def.outputs().begin()->second;
    EXPECT_EQ("/x", topic.name);
    EXPECT_EQ("basic/BoolValue", topic.type.name);
    EXPECT_EQ("a66c7ec12e42a654ff9f38b5e432948449b20f6cc6ca088ae74bbdb0cca34d99", topic.type.hash);
    EXPECT_EQ(448, topic.type.schema.size());
}

}  // namespace ref
