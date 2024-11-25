#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "../src/html_decode.h"

#include <string>
#include <string_view>
using namespace std::literals;

TEST_CASE("HTML Parser check"s) {

  CHECK(HtmlDecode( "test string & other"sv ) == "test string & other"s );
  CHECK(HtmlDecode( ""sv ) == ""s );
  CHECK(HtmlDecode( "&LT&QUOTThe test string&QUOT; &AMP &APOS;other&APOS;&GT"sv ) == R"(<"The test string" & 'other'>)"s );
  CHECK(HtmlDecode( "&lt&quotThe test string&quot; &amp &apos;other&apos;&gt"sv ) == R"(<"The test string" & 'other'>)"s );
  CHECK(HtmlDecode( "&Lt&QuOTThe test string&QuoT; &AmP &&& &ApOS;other&aPOS;&gT"sv ) == "&Lt&QuOTThe test string&QuoT; &AmP &&& &ApOS;other&aPOS;&gT"s );

}