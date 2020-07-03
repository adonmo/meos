#include "../../common/time_utils.hpp"
#include "../../common/utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/time/Period.hpp>

TEST_CASE("periods are validated and constructed properly", "[period]") {
  SECTION("reads from istream") {
    Period period;
    stringstream ss("  [  2012-01-01  ,      2012-01-02 09:40:00+0530 )");
    ss >> period;
    REQUIRE(period.lower() == unix_time_point(2012, 1, 1));
    REQUIRE(period.upper() == unix_time_point(2012, 1, 2, 4, 10));
    REQUIRE(period.lower_inc() == true);
    REQUIRE(period.upper_inc() == false);
  }

  SECTION("all constructors work") {
    unique_ptr<Period> period;
    SECTION("constructors with bounds specified") {
      SECTION("normal constructor") {
        period = make_unique<Period>(unix_time_point(2019, 9, 8),
                                     unix_time_point(2019, 9, 10), false, true);
      }
      SECTION("two string constructor") {
        period = make_unique<Period>("2019-09-08 01:00:00+01",
                                     "2019-09-10 01:00:00+01", false, true);
      }
      SECTION("single string constructor") {
        period = make_unique<Period>(
            "(2019-09-08 01:00:00+01, 2019-09-10 01:00:00+01]");
      }
      REQUIRE(period->lower_inc() == false);
      REQUIRE(period->upper_inc() == true);
    }
    SECTION("constructors with default bounds") {
      SECTION("no strings constructor") {
        period = make_unique<Period>(unix_time_point(2019, 9, 8),
                                     unix_time_point(2019, 9, 10));
      }
      SECTION("two string constructor") {
        period = make_unique<Period>("2019-09-08 01:00:00+01",
                                     "2019-09-10 01:00:00+01");
      }
      REQUIRE(period->lower_inc() == true);
      REQUIRE(period->upper_inc() == false);
    }
    REQUIRE(period->lower() == unix_time_point(2019, 9, 8));
    REQUIRE(period->upper() == unix_time_point(2019, 9, 10));
  }

  SECTION("clearly valid") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto lower = unix_time_point(2012, 1, 1);
    auto upper = unix_time_point(2012, 2, 1);
    Period period(lower, upper, lower_inc, upper_inc);
    REQUIRE(period.lower() == lower);
    REQUIRE(period.upper() == upper);
    REQUIRE(period.lower_inc() == lower_inc);
    REQUIRE(period.upper_inc() == upper_inc);
  }

  SECTION("clearly invalid") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto lower = unix_time_point(2012, 2, 1);
    auto upper = unix_time_point(2012, 1, 1);
    REQUIRE_THROWS_AS((Period{lower, upper, lower_inc, upper_inc}),
                      std::invalid_argument);
  }

  SECTION("edge case where lower == upper") {
    time_point t;
    bool lower_inc, upper_inc, should_be_valid;
    std::tie(t, lower_inc, upper_inc, should_be_valid) =
        GENERATE(table<time_point, bool, bool, bool>({
            {unix_time_point(2012, 1, 1), true, true, true},
            {unix_time_point(2012, 1, 1), true, false, false},
            {unix_time_point(2012, 1, 1), false, false, false},
            {unix_time_point(2012, 1, 1), false, true, false},
        }));
    if (!should_be_valid) {
      REQUIRE_THROWS_AS((Period{t, t, lower_inc, upper_inc}),
                        std::invalid_argument);
    } else {
      REQUIRE_NOTHROW(Period{t, t, lower_inc, upper_inc});
    }
  }
}

TEST_CASE("Period timespan", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto left =
      GENERATE(take(4, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_t const minute = 60 * 1000;
  time_t const year = 365 * 24 * 60 * 60 * 1000L;
  auto duration =
      std::chrono::milliseconds(GENERATE(take(4, random(minute, year))));
  auto left_tp = std::chrono::system_clock::from_time_t(left / 1000L);
  auto period =
      *make_unique<Period>(left_tp, left_tp + duration, lower_inc, upper_inc)
           .get();
  REQUIRE(period.timespan() == duration);
}

TEST_CASE("Period shift", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto period_in =
      *make_unique<Period>(unix_time_point(2012, 1, 1),
                           unix_time_point(2012, 1, 7), lower_inc, upper_inc)
           .get();
  auto day = std::chrono::milliseconds(24 * 60 * 60 * 1000L);
  auto period_out = *period_in.shift(day).get();
  REQUIRE(period_out.lower() == unix_time_point(2012, 1, 2));
  REQUIRE(period_out.upper() == unix_time_point(2012, 1, 8));
  REQUIRE(period_out.lower_inc() == lower_inc);
  REQUIRE(period_out.upper_inc() == upper_inc);
}

TEST_CASE("Period overlap", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  SECTION("clear overlap") {
    auto period_1 =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 4, 1), lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time_point(2012, 2, 1),
                             unix_time_point(2012, 5, 1), lower_inc, upper_inc)
             .get();
    REQUIRE(period_1.overlap(period_2));
  }
  SECTION("clearly no overlap") {
    auto period_1 =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 2, 1), lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time_point(2012, 4, 1),
                             unix_time_point(2012, 5, 1), lower_inc, upper_inc)
             .get();
    REQUIRE(!period_1.overlap(period_2));
  }
  SECTION("borderline overlap case when only both periods are inclusive") {
    auto period_1 =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 3, 1), lower_inc, upper_inc)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time_point(2012, 3, 1),
                             unix_time_point(2012, 5, 1), lower_inc, upper_inc)
             .get();
    bool expected = period_1.upper_inc() && period_2.lower_inc();
    REQUIRE(period_1.overlap(period_2) == expected);
  }
}

TEST_CASE("Period contains timestamp", "[period]") {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  SECTION("clearly contains timestamp") {
    auto period =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 4, 1), lower_inc, upper_inc)
             .get();
    REQUIRE(period.contains_timestamp(unix_time_point(2012, 2, 1)));
    REQUIRE(period.contains_timestamp(unix_time_point(2012, 3, 1)));
  }
  SECTION("clearly does not contains timestamp") {
    auto period =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 4, 1), lower_inc, upper_inc)
             .get();
    REQUIRE(!period.contains_timestamp(unix_time_point(2011, 2, 1)));
    REQUIRE(!period.contains_timestamp(unix_time_point(2012, 5, 1)));
    REQUIRE(!period.contains_timestamp(unix_time_point(2013, 2, 1)));
  }
  SECTION("borderline contains timestamp only on inclusive end") {
    auto period =
        *make_unique<Period>(unix_time_point(2012, 1, 1),
                             unix_time_point(2012, 4, 1), lower_inc, upper_inc)
             .get();
    REQUIRE(period.lower_inc() ==
            period.contains_timestamp(unix_time_point(2012, 1, 1)));
    REQUIRE(period.upper_inc() ==
            period.contains_timestamp(unix_time_point(2012, 4, 1)));
  }
}
