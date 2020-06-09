#include "../../catch.hpp"
#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <meos/types/temporal/TInstant.hpp>

const time_t day = 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstant duration function returns Instant", "[tinst]", int,
                   float, bool) {
  TInstant<TestType> instant(1, unix_time(2012, 1, 1));
  REQUIRE(instant.duration() == "Instant");
}

TEMPLATE_TEST_CASE("TInstant instant functions", "[tinst]", int, float) {
  TestType v = random() % 1000;
  long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
  auto actual = TInstant<TestType>(v, t);

  REQUIRE(actual.numInstants() == 1);

  set<TInstant<TestType>> instants = actual.instants();
  set<TInstant<TestType>> expected = {actual};
  REQUIRE_THAT(instants, UnorderedEquals(expected));

  REQUIRE(actual.startInstant() == actual);
  REQUIRE(actual.endInstant() == actual);
}

TEMPLATE_TEST_CASE("TInstant timespan", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  REQUIRE(instant.timespan() == 0);
}

TEMPLATE_TEST_CASE("TInstant getTime", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  Period p(unix_time(2012, 11, 1), unix_time(2012, 11, 1), true, true);
  set<Period> periods = {p};
  PeriodSet expected(periods);
  REQUIRE(instant.getTime() == expected);
}

TEMPLATE_TEST_CASE("TInstant period", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  Period expected(unix_time(2012, 11, 1), unix_time(2012, 11, 1), true, true);
  REQUIRE(instant.period() == expected);
}

TEMPLATE_TEST_CASE("TInstant shift", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  unique_ptr<TInstant<TestType>> shifted = instant.shift(day);
  REQUIRE(shifted->getTimestamp() == unix_time(2012, 11, 2));
}

TEMPLATE_TEST_CASE("TInstant intersection functions", "[tinstantset]", int,
                   float) {
  TInstant<TestType> instant(10, unix_time(2012, 1, 2));

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(instant.intersectsTimestamp(unix_time(2012, 1, 2)) == true);

    // Negative cases
    REQUIRE(instant.intersectsTimestamp(unix_time(2012, 1, 1)) == false);
    REQUIRE(instant.intersectsTimestamp(unix_time(2012, 1, 2, 1)) == false);
    REQUIRE(instant.intersectsTimestamp(unix_time(2012, 2, 2)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 1, 12), unix_time(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, false)) == true);

    // Negative cases
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), false, true)) == false);
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), false, false)) == false);
    REQUIRE(instant.intersectsPeriod(Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_t> s = {unix_time(2012, 1, 2)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time(2012, 1, 2), unix_time(2012, 1, 5)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time(2012, 1, 1)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time(2012, 1, 2, 1)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time(2012, 1, 4, 1), unix_time(2012, 2, 2)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {
        Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true),
         Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)};

    // Negative cases
    s = {Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}
