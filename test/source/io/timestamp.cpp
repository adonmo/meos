#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/util/serializing.hpp>
#include <string>

#include "../common/time_utils.hpp"

using namespace meos;
using namespace std;

TEST_CASE("timestamps are deserialized", "[deserializer][timestamp]") {
  SECTION("only one timestamp present") {
    Deserializer<int> r("2012-11-01");

    REQUIRE(r.nextTime() == unix_time_point(2012, 11, 1));
    CHECK_THROWS(r.nextTime());
  }

  SECTION("timestamps are parsed properly") {
    string s;
    time_point expected;
    std::tie(s, expected) = GENERATE(table<string, time_point>({
        {"2012-11-01", unix_time_point(2012, 11, 1)},
        {"2012-11-02", unix_time_point(2012, 11, 2)},
        {"2012-11-02 00:00:00", unix_time_point(2012, 11, 2)},
        {"2012-11-02 00:00:00+0000", unix_time_point(2012, 11, 2)},
        {"2012-11-02 01:44:32.789+0000", unix_time_point(2012, 11, 2, 1, 44, 32, 789)},
        {"2012-11-02 01:00:00+01", unix_time_point(2012, 11, 2)},
        {"2012-11-02 01:00:00+0100", unix_time_point(2012, 11, 2)},
        {"2012-11-02 01:30:00+0130", unix_time_point(2012, 11, 2)},
        {"2012-11-02 00:00:00-01", unix_time_point(2012, 11, 2, 1)},
        {"2012-11-02 00:00:00-0100", unix_time_point(2012, 11, 2, 1)},
        {"2012-11-02 00:00:00-0130", unix_time_point(2012, 11, 2, 1, 30)},
    }));
    Deserializer<int> r(s);
    REQUIRE(r.nextTime() == expected);
  }

  SECTION("multiple timestamps present") {
    Deserializer<int> r("2012-11-01\n2012-12-02");

    REQUIRE(r.nextTime() == unix_time_point(2012, 11, 1));
    REQUIRE(r.nextTime() == unix_time_point(2012, 12, 2));
    CHECK_THROWS(r.nextTime());
  }
}

TEST_CASE("timestamps are serialized properly", "[serializer][timestamp]") {
  time_point t;
  string expected;
  std::tie(t, expected) = GENERATE(table<time_point, string>({
      {unix_time_point(2012, 11, 2), "2012-11-02T00:00:00+0000"},
      {unix_time_point(2012, 11, 2, 1, 44, 32, 789), "2012-11-02T01:44:32.789+0000"},
  }));
  REQUIRE(write_ISO8601_time(t) == expected);
}
