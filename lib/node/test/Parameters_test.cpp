#include "JsonParse.h"

#include <gtest/gtest.h>
#include <node/Parameters.h>

namespace ref {

TEST(Parameters, Read) {
    std::string jsonStr =
            "{\"a\":true,\"b\":1,\"c\":1.5,\"d\":\"!\",\"e\":[false,true],\"f\":[0,1],\"g\":[0.5,-0.5],\"h\":[\"1\",\"2\"]}";
    Json::Value json = ParseJSON(jsonStr);

    std::vector<bool> boolArray{false, true};
    std::vector<int> intArray{0, 1};
    std::vector<double> doubleArray{0.5, -0.5};
    std::vector<std::string> stringArray{"1", "2"};

    Parameters p(json);
    EXPECT_EQ(8, p.size());
    EXPECT_EQ(true, p.readBool("a"));
    EXPECT_EQ(1, p.readInt("b"));
    EXPECT_DOUBLE_EQ(1.5, p.readDouble("c"));
    EXPECT_EQ("!", p.readString("d"));
    EXPECT_EQ(boolArray, p.readArrayBool("e"));
    EXPECT_EQ(intArray, p.readArrayInt("f"));
    EXPECT_EQ(doubleArray, p.readArrayDouble("g"));
    EXPECT_EQ(stringArray, p.readArrayString("h"));
}

TEST(Parameters, DefaultValues) {
    Json::Value json = ParseJSON("42");

    Parameters p(json);
    EXPECT_EQ(false, p.readBool("x"));
    EXPECT_EQ(false, p.readBool("x", false));
    EXPECT_EQ(true, p.readBool("x", true));
    EXPECT_EQ(int32_t(0), p.readInt("x"));
    EXPECT_EQ(int32_t(0), p.readInt("x", 0));
    EXPECT_EQ(int32_t(-1), p.readInt("x", -1));
    EXPECT_DOUBLE_EQ(0.0, p.readDouble("x"));
    EXPECT_DOUBLE_EQ(0.0, p.readDouble("x", 0.0));
    EXPECT_DOUBLE_EQ(10.0, p.readDouble("x", 10.0));
    EXPECT_EQ("", p.readString("x"));
    EXPECT_EQ("", p.readString("x", ""));
    EXPECT_EQ(":)", p.readString("x", ":)"));
    EXPECT_TRUE(p.readArrayBool("x").empty());
    EXPECT_TRUE(p.readArrayInt("x").empty());
    EXPECT_TRUE(p.readArrayDouble("x").empty());
    EXPECT_TRUE(p.readArrayString("x").empty());
}

}  // namespace ref
