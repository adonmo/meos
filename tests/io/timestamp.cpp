#include "../catch.hpp"
#include "../common/time_utils.hpp"
#include <meos/io/Deserializer.hpp>

TEST_CASE("timestamps are deserialized", "[deserializer][timestamp]") {
  SECTION("only one timestamp present") {
    Deserializer<int> r("2012-11-01");

    REQUIRE(r.nextTime() == unix_time(2012, 11, 1));
    CHECK_THROWS(r.nextTime());
  }

  SECTION("multiple timestamps present") {
    Deserializer<int> r("2012-11-01\n2012-12-02");

    REQUIRE(r.nextTime() == unix_time(2012, 11, 1));
    REQUIRE(r.nextTime() == 1354406400000);
    CHECK_THROWS(r.nextTime());
  }
}
