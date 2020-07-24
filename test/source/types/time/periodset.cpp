#include <catch2/catch.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <sstream>
#include <string>

#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

TEST_CASE("PeriodSets are constructed properly", "[periodset]") {
  SECTION("reads from istream") {
    PeriodSet period_set;
    stringstream ss("{  [  2012-01-01  ,      2012-01-02 09:40:00+0530 )}");
    ss >> period_set;
    REQUIRE(period_set.periods().size() == 1);
    Period period = period_set.startPeriod();
    REQUIRE(period.lower() == unix_time_point(2012, 1, 1));
    REQUIRE(period.upper() == unix_time_point(2012, 1, 2, 4, 10));
    REQUIRE(period.lower_inc() == true);
    REQUIRE(period.upper_inc() == false);
  }

  SECTION("all constructors work") {
    Period period_1(unix_time_point(2020, 9, 8), unix_time_point(2020, 9, 10));
    Period period_2(unix_time_point(2019, 9, 8), unix_time_point(2019, 9, 10));
    SECTION("no strings constructor") {
      Period period_3(unix_time_point(2019, 9, 8), unix_time_point(2019, 9, 10));  // Duplicate!
      set<Period> s = {period_1, period_2, period_3};
      PeriodSet period_set(s);
      REQUIRE(period_set.periods().size() == 2);
      // We gave the periods out-of-order!
      REQUIRE(period_set.startPeriod() == period_2);
      REQUIRE(period_set.endPeriod() == period_1);
    }
    SECTION("string constructor") {
      PeriodSet period_set(
          "{[2020-09-08 01:00:00+01, 2020-09-10 01:00:00+01), "
          "[2019-09-08 01:00:00+01, 2019-09-10 01:00:00+01)}");
      REQUIRE(period_set.periods().size() == 2);
      // We gave the periods out-of-order!
      REQUIRE(period_set.startPeriod() == period_2);
      REQUIRE(period_set.endPeriod() == period_1);
    }
    SECTION("set of strings constructor") {
      PeriodSet period_set(set<string>{"[2020-09-08 01:00:00+01, 2020-09-10 01:00:00+01)",
                                       "[2019-09-08 01:00:00+01, 2019-09-10 01:00:00+01)"});
      REQUIRE(period_set.periods().size() == 2);
      // We gave the periods out-of-order!
      REQUIRE(period_set.startPeriod() == period_2);
      REQUIRE(period_set.endPeriod() == period_1);
    }
  }
}

TEST_CASE("PeriodSet period functions", "[periodset]") {
  set<Period> periods;
  set<Period> expected_periods;

  size_t size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    bool lower_inc = random() % 2;
    bool upper_inc = random() % 2;
    auto lbound = std::chrono::system_clock::from_time_t(
        (unix_time(2012, 1, 1) + 10 * 365 * (random() % day)) / 1000L);
    auto duration = std::chrono::milliseconds(day + 6 * (random() % day));
    auto rbound = lbound + duration;  // This is to make sure lbound <= rbound
    auto period = Period(lbound, rbound, lower_inc, upper_inc);
    expected_periods.insert(period);
    periods.insert(period);
  }

  PeriodSet actual(periods);
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

TEST_CASE("PeriodSet period gaps are ignored", "[periodset]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  set<Period> periods = {
      Period(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 2), lower_inc, true),
      Period(unix_time_point(2012, 1, 6), unix_time_point(2012, 1, 7), true, upper_inc),
  };
  PeriodSet period_set(periods);
  Period expected
      = Period(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 7), lower_inc, upper_inc);
  REQUIRE(period_set.period() == expected);
}

TEST_CASE("PeriodSet timespan", "[periodset]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto left = std::chrono::system_clock::from_time_t(
      (GENERATE(take(4, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))))) / 1000);
  auto duration_1 = std::chrono::milliseconds(GENERATE(take(4, random(minute, year))));
  auto duration_2 = std::chrono::milliseconds(GENERATE(take(4, random(minute, year))));
  auto period_1 = make_unique<Period>(left, left + duration_1, lower_inc, upper_inc);
  auto period_2 = make_unique<Period>(left, left + duration_2, lower_inc, upper_inc);

  set<unique_ptr<Period>> periods;
  periods.insert(move(period_1));
  periods.insert(move(period_2));
  PeriodSet period_set(periods);

  REQUIRE(period_set.timespan() == (duration_1 + duration_2));
}

TEST_CASE("PeriodSet shift", "[periodset]") {
  set<unique_ptr<Period>> expected_periods;
  set<unique_ptr<Period>> actual_periods;
  size_t size = GENERATE(take(4, random(1, 2)));
  auto shift = std::chrono::milliseconds(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    bool lower_inc = random() % 2;
    bool upper_inc = random() % 2;
    auto lbound = std::chrono::system_clock::from_time_t(
        (unix_time(2012, 1, 1) + 10 * 365 * (random() % day)) / 1000L);
    auto duration = std::chrono::milliseconds(day + 10 * 365 * (random() % day));
    auto rbound = lbound + duration;  // This is to make sure lbound <= rbound
    auto period = make_unique<Period>(lbound, rbound, lower_inc, upper_inc);
    actual_periods.insert(period->shift(shift));
    auto expected_period
        = make_unique<Period>(lbound + shift, rbound + shift, lower_inc, upper_inc);
    expected_periods.insert(move(expected_period));
  }

  PeriodSet actual(actual_periods);
  PeriodSet expected(expected_periods);
  REQUIRE_THAT(actual.periods(), UnorderedEquals(expected.periods()));
}

TEST_CASE("PeriodSet timestamp functions", "[periodset]") {
  set<Period> periods;
  set<time_point> expected_timestamps;
  size_t size = GENERATE(0, take(4, random(1, 3)));

  for (size_t i = 0; i < size; i++) {
    bool lower_inc = random() % 2;
    bool upper_inc = random() % 2;
    time_point lbound = std::chrono::system_clock::from_time_t(
        (unix_time(2012, 1, 1) + 10 * 365 * (random() % day)) / 1000L);
    duration_ms duration(day + 6 * (random() % day));
    auto rbound = lbound + duration;  // This is to make sure lbound <= rbound
    auto period = Period(lbound, rbound, lower_inc, upper_inc);
    expected_timestamps.insert(lbound);  // Round off to seconds
    expected_timestamps.insert(rbound);  // Round off to seconds
    periods.insert(period);
  }

  PeriodSet actual(periods);
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
