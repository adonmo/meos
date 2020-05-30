#include "../catch.hpp"
#include "../common/matchers.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <string>

using namespace std;

const long millis_in_day = 86400000L;

/**
 * Splitting a serialized period set is not straight forward.
 * This is because we can't just split based on ", " as the delimiter
 * as even periods include it.
 */
set<string> split_period_set_into_periods(const string &period_set_str) {
  const string delim = ", ";
  set<string> tokens;
  size_t prev = 0, pos = 0;
  do {
    // find twice to skip alternative ", "s
    pos = period_set_str.find(delim, prev);
    pos = period_set_str.find(delim, pos + 1);
    if (pos == string::npos)
      pos = period_set_str.length();
    string token = period_set_str.substr(prev, pos - prev);
    if (!token.empty())
      tokens.insert(token);
    prev = pos + delim.length();
  } while (pos < period_set_str.length() && prev < period_set_str.length());
  return tokens;
}

TEST_CASE("PeriodSets are serialized", "[serializer][periodset]") {
  Serializer<> w;
  SECTION("empty set") {
    string expected = "{}";

    set<unique_ptr<Period>> periods;
    PeriodSet period_set(periods);

    REQUIRE(w.write(&period_set) == expected);
  }

  SECTION("set with only one value") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    long lbound = GENERATE(0L, unix_time(2012, 1, 1),
                           take(10, random(0L, 4102488000000L)));
    long duration = GENERATE(take(10, random(0L, 10 * 365 * millis_in_day)));
    auto rbound = lbound + duration; // This is to make sure lbound <= rbound

    auto period = make_unique<Period>(lbound, rbound, left_open, right_open);
    string expected = "{" + w.write(period.get()) + "}";

    set<unique_ptr<Period>> periods;
    periods.insert(move(period));
    PeriodSet period_set(periods);

    REQUIRE(w.write(&period_set) == expected);
  }

  SECTION("set with only multiple values") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto size = GENERATE(take(4, random(1, 2)));

    set<string> expected;
    set<unique_ptr<Period>> periods;

    for (size_t i = 0; i < size; i++) {
      long lbound = GENERATE(0L, unix_time(2012, 1, 1),
                             take(4, random(0L, 4102488000000L)));
      long duration = GENERATE(take(4, random(0L, 10 * 365 * millis_in_day)));
      auto rbound = lbound + duration; // This is to make sure lbound <= rbound
      auto period = make_unique<Period>(lbound, rbound, left_open, right_open);
      expected.insert(w.write(period.get()));

      periods.insert(move(period));
    }

    PeriodSet period_set(periods);

    string serialized = w.write(&period_set);

    REQUIRE(serialized.size() > 2);
    REQUIRE(serialized[0] == '{');
    REQUIRE(serialized[serialized.size() - 1] == '}');
    set<string> actual = split_period_set_into_periods(
        serialized.substr(1, serialized.size() - 2));
    REQUIRE_THAT(actual, UnorderedEquals(expected));
  }
}

TEST_CASE("PeriodSets are deserialized", "[deserializer][periodset]") {
  SECTION("only one PeriodSet present") {
    SECTION("only one inst present") {
      Deserializer<> r("{[2012-11-01, 2012-04-01]}");

      unique_ptr<PeriodSet> period_set = r.nextPeriodSet();
      set<Period> actual = unwrap(period_set->periods);
      set<Period> expected = {
          Period(unix_time(2012, 11, 1), unix_time(2012, 4, 1), false, false)};
      auto x = UnorderedEquals(expected);
      REQUIRE(actual.size() == 1);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextPeriodSet());
    }

    SECTION("multiple inst present") {
      Deserializer<> r("{(2012-01-01 00:00:00+00, 2012-04-01 00:00:00+00), "
                       "[2012-01-01 00:00:00+00, 2012-04-01 00:00:00+00)}");

      unique_ptr<PeriodSet> period_set = r.nextPeriodSet();
      set<Period> actual = unwrap(period_set->periods);
      set<Period> expected = {
          Period(unix_time(2012, 1, 1), unix_time(2012, 4, 1), true, true),
          Period(unix_time(2012, 1, 1), unix_time(2012, 4, 1), false, true),
      };
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextPeriodSet());
    }
  }

  SECTION("multiple PeriodSets present") {
    Deserializer<> r(
        "{(2012-01-01 00:00:00+00, 2012-04-01 00:00:00+00)}{[2012-01-01 "
        "00:00:00+00, 2012-04-01 00:00:00+00)}");

    unique_ptr<PeriodSet> period_set_1 = r.nextPeriodSet();
    set<Period> actual_1 = unwrap(period_set_1->periods);
    set<Period> expected_1 = {
        Period(unix_time(2012, 1, 1), unix_time(2012, 4, 1), true, true),
    };
    auto x_1 = UnorderedEquals(expected_1);
    REQUIRE_THAT(actual_1, x_1);

    unique_ptr<PeriodSet> period_set_2 = r.nextPeriodSet();
    set<Period> actual_2 = unwrap(period_set_2->periods);
    set<Period> expected_2 = {
        Period(unix_time(2012, 1, 1), unix_time(2012, 4, 1), false, true),
    };
    auto x_2 = UnorderedEquals(expected_2);
    REQUIRE_THAT(actual_2, x_2);

    CHECK_THROWS(r.nextPeriodSet());
  }
}
