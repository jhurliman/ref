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
    EXPECT_EQ(1, def.outputs().size());
}

}  // namespace ref
