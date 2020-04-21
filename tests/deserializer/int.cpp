#include "../catch.hpp"
#include <meos/io/Deserializer.hpp>

TEST_CASE("integers are deserialized", "[deserializer][int]") {
  SECTION("only one integer present") {
    Deserializer<int> r("2012");

    REQUIRE(r.nextValue() == 2012);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("multiple integers present") {
    Deserializer<int> r("2012 11 01");

    REQUIRE(r.nextValue() == 2012);
    REQUIRE(r.nextValue() == 11);
    REQUIRE(r.nextValue() == 01);
    CHECK_THROWS(r.nextValue());
  }
}
