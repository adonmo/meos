#include "../../catch.hpp"
#include "../../common/time_utils.hpp"
#include "../../common/utils.hpp"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>

TEST_CASE("Period overlap", "[period]") {
  SECTION("clear overlap") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             left_open, right_open)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 2, 1), unix_time(2012, 5, 1),
                             left_open, right_open)
             .get();
    REQUIRE(period_1.overlap(period_2));
  }
  SECTION("clearly no overlap") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 2, 1),
                             left_open, right_open)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 4, 1), unix_time(2012, 5, 1),
                             left_open, right_open)
             .get();
    REQUIRE(!period_1.overlap(period_2));
  }
  SECTION("borderline overlap case when only both periods are inclusive") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 3, 1),
                             left_open, right_open)
             .get();
    auto period_2 =
        *make_unique<Period>(unix_time(2012, 3, 1), unix_time(2012, 5, 1),
                             left_open, right_open)
             .get();
    bool expected = !period_1.isRightOpen() && !period_2.isLeftOpen();
    REQUIRE(period_1.overlap(period_2) == expected);
  }
}

TEST_CASE("Period contains timestamp", "[period]") {
  SECTION("clearly contains timestamp") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             left_open, right_open)
             .get();
    REQUIRE(period.contains_timestamp(unix_time(2012, 2, 1)));
    REQUIRE(period.contains_timestamp(unix_time(2012, 3, 1)));
  }
  SECTION("clearly does not contains timestamp") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             left_open, right_open)
             .get();
    REQUIRE(!period.contains_timestamp(unix_time(2011, 2, 1)));
    REQUIRE(!period.contains_timestamp(unix_time(2012, 5, 1)));
    REQUIRE(!period.contains_timestamp(unix_time(2013, 2, 1)));
  }
  SECTION("borderline contains timestamp only on inclusive end") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period =
        *make_unique<Period>(unix_time(2012, 1, 1), unix_time(2012, 4, 1),
                             left_open, right_open)
             .get();
    REQUIRE(!period.isLeftOpen() ==
            period.contains_timestamp(unix_time(2012, 1, 1)));
    REQUIRE(!period.isRightOpen() ==
            period.contains_timestamp(unix_time(2012, 4, 1)));
  }
}
