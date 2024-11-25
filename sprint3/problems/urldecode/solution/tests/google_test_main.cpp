#include <gtest/gtest.h>


#include "../src/url_decode.h"

TEST(UrlDecode_tests, Basic_tests) {
  EXPECT_EQ(UrlDecode(""), std::string());
  EXPECT_EQ(UrlDecode("simple+space"), std::string("simple space"));
  EXPECT_EQ(UrlDecode("%48%65llo+%77orld!"), std::string("Hello world!"));
  EXPECT_NE(UrlDecode("%48%65llo_%77orld!"), std::string("Hello world!"));
  EXPECT_THROW(UrlDecode("%Rte"), std::invalid_argument);
  EXPECT_EQ(UrlDecode("%4Brt"), std::string("Krt"));
}
