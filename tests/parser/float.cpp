#include "../catch.hpp"
#include <meos/io/Parser.hpp>

TEST_CASE("floats are parsed", "[parser][float]") {
  SECTION("only one float present") {
    Parser<float> r("20.12");

    REQUIRE(r.parseNextValue() == 20.12f);
    CHECK_THROWS(r.parseNextValue());
  }

  SECTION("multiple floats present") {
    Parser<float> r("20.12 11 0.1");

    REQUIRE(r.parseNextValue() == 20.12f);
    REQUIRE(r.parseNextValue() == 11.0f);
    REQUIRE(r.parseNextValue() == 0.1f);
    CHECK_THROWS(r.parseNextValue());
  }
}
