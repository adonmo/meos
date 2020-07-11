#include "../common/matchers.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"
#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <string>

using namespace std;

TEST_CASE("TimestampSets are serialized", "[serializer][timestampset]") {
  Serializer<> w;
  SECTION("empty set") {
    string expected = "{}";

    set<time_point> timestamps;
    TimestampSet timestamp_set(timestamps);

    REQUIRE(w.write(&timestamp_set) == expected);
  }

  SECTION("set with only one value") {
    time_t t = GENERATE(0L, unix_time(2012, 1, 1),
                        take(10, random(0L, 4102488000000L)));
    time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);

    string expected = "{" + w.writeTime(tp) + "}";

    set<time_point> timestamps;
    timestamps.insert(tp);
    TimestampSet timestamp_set(timestamps);

    REQUIRE(w.write(&timestamp_set) == expected);
  }

  SECTION("set with multiple values") {
    size_t size = GENERATE(take(4, random(1, 2)));

    set<string> expected;
    set<time_point> timestamps;

    for (size_t i = 0; i < size; i++) {
      time_t t = GENERATE(0L, unix_time(2012, 1, 1),
                          take(4, random(0L, 4102488000000L)));
      time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);
      expected.insert(w.writeTime(tp));

      timestamps.insert(tp);
    }

    TimestampSet timestamp_set(timestamps);

    string serialized = w.write(&timestamp_set);

    REQUIRE(serialized.size() > 2);
    REQUIRE(serialized[0] == '{');
    REQUIRE(serialized[serialized.size() - 1] == '}');
    set<string> actual =
        split_into_set(serialized.substr(1, serialized.size() - 2), ", ");
    REQUIRE_THAT(actual, UnorderedEquals(expected));
  }
}

TEST_CASE("TimestampSets are deserialized", "[deserializer][timestampset]") {
  SECTION("only one TimestampSet present") {
    SECTION("only one inst present") {
      Deserializer<> r("{2012-11-01}");

      unique_ptr<TimestampSet> timestamp_set = r.nextTimestampSet();
      set<time_point> actual = timestamp_set->timestamps();
      set<time_point> expected = {unix_time_point(2012, 11, 1)};
      auto x = UnorderedEquals(expected);
      REQUIRE(actual.size() == 1);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTimestampSet());
    }

    SECTION("multiple inst present") {
      Deserializer<> r("{2012-01-01 00:00:00+00, 2012-04-01 00:00:00+00}");

      unique_ptr<TimestampSet> timestamp_set = r.nextTimestampSet();
      set<time_point> actual = timestamp_set->timestamps();
      set<time_point> expected = {
          unix_time_point(2012, 1, 1),
          unix_time_point(2012, 4, 1),
      };
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTimestampSet());
    }
  }

  SECTION("multiple TimestampSets present") {
    Deserializer<> r("{2012-01-01 00:00:00+00} {2012-04-01 00:00:00+00}");

    unique_ptr<TimestampSet> timestamp_set_1 = r.nextTimestampSet();
    set<time_point> actual_1 = timestamp_set_1->timestamps();
    set<time_point> expected_1 = {unix_time_point(2012, 1, 1)};
    auto x_1 = UnorderedEquals(expected_1);
    REQUIRE_THAT(actual_1, x_1);

    unique_ptr<TimestampSet> timestamp_set_2 = r.nextTimestampSet();
    set<time_point> actual_2 = timestamp_set_2->timestamps();
    set<time_point> expected_2 = {unix_time_point(2012, 4, 1)};
    auto x_2 = UnorderedEquals(expected_2);
    REQUIRE_THAT(actual_2, x_2);

    CHECK_THROWS(r.nextTimestampSet());
  }
}
