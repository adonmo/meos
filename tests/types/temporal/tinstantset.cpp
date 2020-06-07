#include "../../catch.hpp"
#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <meos/types/temporal/TInstantSet.hpp>

const time_t minute = 60 * 1000L;
const time_t day = 24 * 60 * 60 * 1000L;
const time_t year = 365 * 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstantSet instant functions", "[tinstantset]", int,
                   float) {
  set<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  auto size = GENERATE(0, take(4, random(1, 6)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
    expected_instants.insert(instant);
  }

  TInstantSet<TestType> actual(instants);
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

TEMPLATE_TEST_CASE("TInstantSet period and timestamp related functions",
                   "[tinstantset]", int, float) {
  set<TInstant<TestType>> instants;

  auto size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
  }

  TInstantSet<TestType> actual(instants);
  REQUIRE_THAT(actual.getInstants(), UnorderedEquals(instants));
  if (size > 0) {
    Period p = actual.period();
    REQUIRE(p.lower() == actual.startTimestamp());
    REQUIRE(p.upper() == actual.endTimestamp());
    REQUIRE(p.lower_inc() == true);
    REQUIRE(p.upper_inc() == true);
  } else {
    CHECK_THROWS(actual.period());
  }
}

TEMPLATE_TEST_CASE("TInstantSet.period() - gaps are ignored", "[tinstantset]",
                   int, float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(1, unix_time(2012, 1, 1)),
      TInstant<TestType>(2, unix_time(2012, 1, 2)),
      TInstant<TestType>(3, unix_time(2012, 1, 6)),
      TInstant<TestType>(4, unix_time(2012, 1, 7)),
  };
  TInstantSet<TestType> instant_set(instants);
  Period expected =
      Period(unix_time(2012, 1, 1), unix_time(2012, 1, 7), true, true);
  REQUIRE(instant_set.period() == expected);
}

TEMPLATE_TEST_CASE("TInstantSet timespan", "[tinstantset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto period_1 = make_unique<TInstant<TestType>>(v1, t1);
  auto period_2 = make_unique<TInstant<TestType>>(v2, t2);

  set<unique_ptr<TInstant<TestType>>> instants;
  instants.insert(move(period_1));
  instants.insert(move(period_2));
  TInstantSet<TestType> instant_set(instants);

  REQUIRE(instant_set.timespan() ==
          instant_set.endTimestamp() - instant_set.startTimestamp());
}

TEMPLATE_TEST_CASE("TInstantSet shift", "[tinstantset]", int, float) {
  set<unique_ptr<TInstant<TestType>>> expected_instants;
  set<unique_ptr<TInstant<TestType>>> actual_instants;
  auto size = GENERATE(take(4, random(1, 2)));
  auto shift = GENERATE(take(4, random(minute, day)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
    auto instant = make_unique<TInstant<TestType>>(v, t);
    actual_instants.insert(instant->shift(shift));
    auto expected_instant = make_unique<TInstant<TestType>>(v, t + shift);
    expected_instants.insert(move(expected_instant));
  }

  TInstantSet<TestType> actual(actual_instants);
  TInstantSet<TestType> expected(expected_instants);
  REQUIRE_THAT(actual.getInstants(), UnorderedEquals(expected.getInstants()));
}
