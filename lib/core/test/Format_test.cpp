#include <core/Format.h>

#include <gtest/gtest.h>

namespace ref {

TEST(Format, Types) {
    const char a = 'a';
    const char* abc = "abc";
    const std::string def = "def";
    const bool b = true;
    const float f = -25.4f;
    const double d = 42.01;

    EXPECT_EQ("a abcdef true -25.4 42.01", Format("%s %s%s %s %s %s", a, abc, def, b, f, d));
}

TEST(Format, Float) {
    EXPECT_EQ("00.00450", Format("%08.5f", 0.0045));
}

}  // namespace ref
