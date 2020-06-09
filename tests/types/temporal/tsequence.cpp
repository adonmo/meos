#include "../../catch.hpp"
#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <meos/types/temporal/TSequence.hpp>

const time_t minute = 60 * 1000L;
const time_t day = 24 * 60 * 60 * 1000L;
const time_t year = 365 * 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TSequence duration function returns Sequence",
                   "[tsequence]", int, float, bool, string, Geometry) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> v;
  TSequence<TestType> sequence(v, lower_inc, upper_inc);
  REQUIRE(sequence.duration() == "Sequence");
}

TEMPLATE_TEST_CASE("TSequence instant functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  auto size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = TInstant<TestType>(v, t);
    instants.push_back(instant);
    expected_instants.insert(instant);
  }

  TSequence<TestType> actual(instants, lower_inc, upper_inc);
  REQUIRE(actual.numInstants() == expected_instants.size());
  REQUIRE_THAT(actual.instants(), UnorderedEquals(expected_instants));
  if (size > 0) {
    REQUIRE(actual.startInstant() == *expected_instants.begin());
    REQUIRE(actual.endInstant() == *expected_instants.rbegin());
  } else {
    CHECK_THROWS(actual.startInstant());
    CHECK_THROWS(actual.endInstant());
  }
}

TEMPLATE_TEST_CASE("TSequence getTime", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto instant_1 = make_unique<TInstant<TestType>>(v1, t1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, t2);

  vector<unique_ptr<TInstant<TestType>>> instants;
  instants.push_back(move(instant_1));
  instants.push_back(move(instant_2));
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  auto period = sequence.period();
  set<Period> periods = {period};
  PeriodSet expected(periods);

  REQUIRE(sequence.getTime() == expected);
}

TEMPLATE_TEST_CASE("TSequence period and timestamp related functions",
                   "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> instants;

  auto size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = TInstant<TestType>(v, t);
    instants.push_back(instant);
  }

  TSequence<TestType> actual(instants, lower_inc, upper_inc);
  REQUIRE_THAT(actual.getInstants(), Catch::Matchers::Equals(instants));
  if (size > 0) {
    Period p = actual.period();
    REQUIRE(p.lower() == actual.startTimestamp());
    REQUIRE(p.upper() == actual.endTimestamp());
    REQUIRE(p.lower_inc() == lower_inc);
    REQUIRE(p.upper_inc() == upper_inc);
  } else {
    CHECK_THROWS(actual.period());
  }
}

TEMPLATE_TEST_CASE("TSequence.period() - gaps are ignored", "[tsequence]", int,
                   float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> instants = {
      TInstant<TestType>(1, unix_time(2012, 1, 1)),
      TInstant<TestType>(2, unix_time(2012, 1, 2)),
      TInstant<TestType>(3, unix_time(2012, 1, 6)),
      TInstant<TestType>(4, unix_time(2012, 1, 7)),
  };
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);
  Period expected = Period(unix_time(2012, 1, 1), unix_time(2012, 1, 7),
                           lower_inc, upper_inc);
  REQUIRE(sequence.period() == expected);
}

TEMPLATE_TEST_CASE("TSequence timespan", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto instant_1 = make_unique<TInstant<TestType>>(v1, t1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, t2);

  vector<unique_ptr<TInstant<TestType>>> instants;
  instants.push_back(move(instant_1));
  instants.push_back(move(instant_2));
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  REQUIRE(sequence.timespan() ==
          sequence.endTimestamp() - sequence.startTimestamp());
}

TEMPLATE_TEST_CASE("TSequence shift", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);

  vector<unique_ptr<TInstant<TestType>>> expected_instants;
  vector<unique_ptr<TInstant<TestType>>> actual_instants;
  auto size = GENERATE(take(4, random(1, 2)));
  auto shift = GENERATE(take(4, random(minute, day)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = make_unique<TInstant<TestType>>(v, t);
    actual_instants.push_back(instant->shift(shift));
    auto expected_instant = make_unique<TInstant<TestType>>(v, t + shift);
    expected_instants.push_back(move(expected_instant));
  }

  TSequence<TestType> actual(actual_instants, lower_inc, upper_inc);
  TSequence<TestType> expected(expected_instants, lower_inc, upper_inc);
  REQUIRE_THAT(actual.getInstants(),
               Catch::Matchers::Equals(expected.getInstants()));
}

TEMPLATE_TEST_CASE("TSequence intersection functions", "[tsequence]", int,
                   float) {
  vector<TInstant<TestType>> instants = {
      TInstant<TestType>(10, unix_time(2012, 1, 2)),
      TInstant<TestType>(20, unix_time(2012, 1, 3)),
      TInstant<TestType>(30, unix_time(2012, 1, 4)),
  };
  TSequence<TestType> sequence(instants, true, true);

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 2)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 2, 1)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 3)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 4)) == true);

    // Negative cases
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 1)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 4, 1)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 1, 5)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time(2012, 2, 1)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 1, 12), unix_time(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, false)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), false, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), false, false)) == true);

    // Negative cases
    REQUIRE(sequence.intersectsPeriod(Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_t> s = {unix_time(2012, 1, 2)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time(2012, 1, 2, 1)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time(2012, 1, 3), unix_time(2012, 1, 5)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time(2012, 1, 1)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time(2012, 1, 4, 1), unix_time(2012, 2, 2)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {
        Period(unix_time(2012, 1, 2), unix_time(2012, 1, 3), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time(2012, 1, 2), unix_time(2012, 1, 5), true, true),
         Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)};
    s = {Period(unix_time(2012, 1, 3), unix_time(2012, 1, 5), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time(2012, 2, 2), unix_time(2012, 2, 3), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}
