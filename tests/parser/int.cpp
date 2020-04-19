#include "../catch.hpp"
#include <meos/io/Parser.hpp>

TEST_CASE("integers are parsed", "[parser][int]") {
  SECTION("only one integer present") {
    Parser<int> r("2012");

    REQUIRE(r.parseNextValue() == 2012);
    CHECK_THROWS(r.parseNextValue());
  }

  SECTION("multiple integers present") {
    Parser<int> r("2012 11 01");

    REQUIRE(r.parseNextValue() == 2012);
    REQUIRE(r.parseNextValue() == 11);
    REQUIRE(r.parseNextValue() == 01);
    CHECK_THROWS(r.parseNextValue());
  }
}
