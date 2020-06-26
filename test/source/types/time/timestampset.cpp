#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/TimestampSet.hpp>

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

TEST_CASE("TimestampSet period functions", "[timestampset]") {
  set<time_point> timestamps;
  set<Period> expected_periods;

  auto size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    time_t t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);
    timestamps.insert(tp);
    expected_periods.insert(Period(tp, tp, true, true));
  }

  TimestampSet actual(timestamps);
  REQUIRE(actual.numPeriods() == expected_periods.size());
  REQUIRE_THAT(actual.periods(), UnorderedEquals(expected_periods));
  if (size > 0) {
    REQUIRE(actual.startPeriod() == *expected_periods.begin());
    REQUIRE(actual.endPeriod() == *expected_periods.rbegin());
    Period p = actual.period();
    REQUIRE(p.lower() == actual.startPeriod().lower());
    REQUIRE(p.upper() == actual.endPeriod().upper());
    REQUIRE(p.lower_inc() == actual.startPeriod().lower_inc());
    REQUIRE(p.upper_inc() == actual.endPeriod().upper_inc());
  } else {
    CHECK_THROWS(actual.startPeriod());
    CHECK_THROWS(actual.endPeriod());
    CHECK_THROWS(actual.period());
  }
}

TEST_CASE("TimestampSet period gaps are ignored", "[timestampset]") {
  set<time_point> timestamps = {
      unix_time_point(2012, 1, 1),
      unix_time_point(2012, 1, 2),
      unix_time_point(2012, 1, 6),
      unix_time_point(2012, 1, 7),
  };
  TimestampSet period_set(timestamps);
  Period expected = Period(unix_time_point(2012, 1, 1),
                           unix_time_point(2012, 1, 7), true, true);
  REQUIRE(period_set.period() == expected);
}

TEST_CASE("TimestampSet timespan", "[timestampset]") {
  auto size = GENERATE(0, take(4, random(1, 30)));
  set<time_point> timestamps;

  for (size_t i = 0; i < size; i++) {
    time_t t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);
    timestamps.insert(tp);
  }

  TimestampSet timestamp_set(timestamps);
  REQUIRE(timestamp_set.timespan() == duration_ms(0));
}

TEST_CASE("TimestampSet shift", "[timestampset]") {
  auto size = GENERATE(0, take(4, random(1, 20)));
  duration_ms shift(GENERATE(0L, take(4, random(minute, day))));
  set<time_point> expected_timestamps;
  set<time_point> actual_timestamps;

  for (size_t i = 0; i < size; i++) {
    time_t t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);
    actual_timestamps.insert(tp);
    expected_timestamps.insert(tp + shift);
  }

  TimestampSet actual(actual_timestamps);
  TimestampSet expected(expected_timestamps);
  REQUIRE_THAT(actual.shift(shift).get()->timestamps(),
               UnorderedEquals(expected.timestamps()));
}

TEST_CASE("TimestampSet timestamp functions", "[timestampset]") {
  auto size = GENERATE(0, take(4, random(1, 100)));
  set<time_point> expected_timestamps;
  set<time_point> actual_timestamps;

  for (size_t i = 0; i < size; i++) {
    time_t t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    time_point tp = std::chrono::system_clock::from_time_t(t / 1000L);
    actual_timestamps.insert(tp);
    expected_timestamps.insert(tp);
  }

  TimestampSet actual(actual_timestamps);
  REQUIRE(actual.numTimestamps() == expected_timestamps.size());
  REQUIRE_THAT(actual.timestamps(), UnorderedEquals(expected_timestamps));
  if (size > 0) {
    REQUIRE(actual.startTimestamp() == *expected_timestamps.begin());
    REQUIRE(actual.endTimestamp() == *expected_timestamps.rbegin());
  } else {
    CHECK_THROWS(actual.startTimestamp());
    CHECK_THROWS(actual.endTimestamp());
  }
}
