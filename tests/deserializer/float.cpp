#include "../catch.hpp"
#include <meos/io/Deserializer.hpp>

TEST_CASE("floats are deserialized", "[deserializer][float]") {
  SECTION("only one float present") {
    Deserializer<float> r("20.12");

    REQUIRE(r.nextValue() == 20.12f);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("multiple floats present") {
    Deserializer<float> r("20.12 11 0.1");

    REQUIRE(r.nextValue() == 20.12f);
    REQUIRE(r.nextValue() == 11.0f);
    REQUIRE(r.nextValue() == 0.1f);
    CHECK_THROWS(r.nextValue());
  }
}
