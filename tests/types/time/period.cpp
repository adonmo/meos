#include "../../catch.hpp"
#include "../../common/time_utils.hpp"
#include "../../common/utils.hpp"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>

TEST_CASE("Period timespan", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto left =
      GENERATE(take(4, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  const time_t minute = 60 * 1000;
  const time_t year = 365 * 24 * 60 * 60 * 1000L;
  auto duration = GENERATE(take(4, random(minute, year)));
  auto period =
      *make_unique<Period>(left, left + duration, lower_inc, upper_inc).get();
  REQUIRE(period.timespan() == duration);
}

TEST_CASE("Period shift", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto period_in =
      *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 1, 7),
                           lower_inc, upper_inc)
           .get();
  const time_t day = 24 * 60 * 60 * 1000L;
  auto period_out = *period_in.shift(day).get();
  REQUIRE(period_out.lower() == unix_time(2012, 1, 2));
  REQUIRE(period_out.upper() == unix_time(2012, 1, 8));
  REQUIRE(period_out.lower_inc() == lower_inc);
  REQUIRE(period_out.upper_inc() == upper_inc);
}

TEST_CASE("Period overlap", "[period]") {
  SECTION("clear overlap") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 2, 1), unix_time(2012, 5, 1),
                             lower_inc, upper_inc)
             .get();
    REQUIRE(period_1.overlap(period_2));
  }
  SECTION("clearly no overlap") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 2, 1),
                             lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 4, 1), unix_time(2012, 5, 1),
                             lower_inc, upper_inc)
             .get();
    REQUIRE(!period_1.overlap(period_2));
  }
  SECTION("borderline overlap case when only both periods are inclusive") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 3, 1),
                             lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 3, 1), unix_time(2012, 5, 1),
                             lower_inc, upper_inc)
             .get();
    bool expected = period_1.upper_inc() && period_2.lower_inc();
    REQUIRE(period_1.overlap(period_2) == expected);
  }
}

TEST_CASE("Period contains timestamp", "[period]") {
  SECTION("clearly contains timestamp") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             lower_inc, upper_inc)
             .get();
    REQUIRE(period.contains_timestamp(unix_time(2012, 2, 1)));
    REQUIRE(period.contains_timestamp(unix_time(2012, 3, 1)));
  }
  SECTION("clearly does not contains timestamp") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             lower_inc, upper_inc)
             .get();
    REQUIRE(!period.contains_timestamp(unix_time(2011, 2, 1)));
    REQUIRE(!period.contains_timestamp(unix_time(2012, 5, 1)));
    REQUIRE(!period.contains_timestamp(unix_time(2013, 2, 1)));
  }
  SECTION("borderline contains timestamp only on inclusive end") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             lower_inc, upper_inc)
             .get();
    REQUIRE(period.lower_inc() ==
            period.contains_timestamp(unix_time(2012, 1, 1)));
    REQUIRE(period.upper_inc() ==
            period.contains_timestamp(unix_time(2012, 4, 1)));
  }
}
