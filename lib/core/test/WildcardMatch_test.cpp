#include <core/WildcardMatch.h>

#include <gtest/gtest.h>

namespace ref {

TEST(WildcardMatch, BasicMatches) {
    EXPECT_EQ(false, WildcardMatch("abc", "def"));
    EXPECT_EQ(false, WildcardMatch("abc", "a"));
    EXPECT_EQ(false, WildcardMatch("abc", "ab"));
    EXPECT_EQ(true, WildcardMatch("abc", "abc"));
    EXPECT_EQ(false, WildcardMatch("abc", "abcd"));
    EXPECT_EQ(true, WildcardMatch("abc", "a*"));
    EXPECT_EQ(true, WildcardMatch("abc", "a*c"));
    EXPECT_EQ(true, WildcardMatch("abc", "abc*"));
    EXPECT_EQ(true, WildcardMatch("abc", "?bc"));
    EXPECT_EQ(false, WildcardMatch("abc", "?abc"));
    EXPECT_EQ(true, WildcardMatch("ab*", "ab\\*"));
    EXPECT_EQ(true, WildcardMatch("abc", "[xya]bc"));
    EXPECT_EQ(false, WildcardMatch("abc", "[xyz]bc"));
}

}  // namespace ref
