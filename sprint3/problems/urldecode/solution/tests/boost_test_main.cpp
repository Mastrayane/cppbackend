#define BOOST_TEST_MODULE url decode application tests
#include <boost/test/unit_test.hpp>

#include "../src/url_decode.h"



BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
  BOOST_CHECK_EQUAL(UrlDecode(""), std::string());
  BOOST_CHECK_EQUAL(UrlDecode("simple+space"), std::string("simple space"));
  BOOST_CHECK_EQUAL(UrlDecode("%48%65llo+%77orld!"), std::string("Hello world!"));
  BOOST_CHECK_NE(UrlDecode("%48%65llo_%77orld!"), std::string("Hello world!"));
  BOOST_CHECK_THROW(UrlDecode("%Rte"), std::invalid_argument);
  BOOST_CHECK_EQUAL(UrlDecode("%4Brt"), std::string("Krt"));
}