#include "../../catch.hpp"
#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include "../../common/utils.hpp"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>

const time_t minute = 60 * 1000;
const time_t day = 24 * 60 * 60 * 1000L;
const time_t year = 365 * 24 * 60 * 60 * 1000L;

TEST_CASE("PeriodSet timespan", "[periodset]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto left =
      GENERATE(take(4, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto duration_1 = GENERATE(take(4, random(minute, year)));
  auto duration_2 = GENERATE(take(4, random(minute, year)));
  auto period_1 =
      make_unique<Period>(left, left + duration_1, lower_inc, upper_inc);
  auto period_2 =
      make_unique<Period>(left, left + duration_2, lower_inc, upper_inc);

  set<unique_ptr<Period>> periods;
  periods.insert(move(period_1));
  periods.insert(move(period_2));
  PeriodSet period_set(periods);

  REQUIRE(period_set.timespan() == duration_1 + duration_2);
}

TEST_CASE("PeriodSet shift", "[periodset]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto size = GENERATE(take(4, random(1, 2)));

  set<unique_ptr<Period>> expected_periods;
  set<unique_ptr<Period>> actual_periods;
  auto shift = GENERATE(take(4, random(minute, day)));

  for (size_t i = 0; i < size; i++) {
    long lbound = GENERATE(0L, unix_time(2012, 1, 1),
                           take(4, random(0L, 4102488000000L)));
    long duration = GENERATE(take(4, random(0L, 10 * 365 * day)));
    auto rbound = lbound + duration; // This is to make sure lbound <= rbound
    auto period = make_unique<Period>(lbound, rbound, lower_inc, upper_inc);
    actual_periods.insert(period->shift(shift));
    auto expected_period = make_unique<Period>(lbound + shift, rbound + shift,
                                               lower_inc, upper_inc);
    expected_periods.insert(move(expected_period));
  }

  PeriodSet actual(actual_periods);
  PeriodSet expected(expected_periods);
  REQUIRE_THAT(actual.getPeriods(), UnorderedEquals(expected.getPeriods()));
}
