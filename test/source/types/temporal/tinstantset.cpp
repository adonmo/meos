#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/temporal/TInstantSet.hpp>

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstantSet comparision operators", "[tinstset]", int,
                   float) {
  SECTION("lhs == rhs") {
    set<TInstant<TestType>> lhs_instants = {
        TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
        TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
        TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
        TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
    };
    TInstantSet<TestType> lhs(lhs_instants);
    set<TInstant<TestType>> rhs_instants = {
        TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
        TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
        TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
        TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
    };
    TInstantSet<TestType> rhs(rhs_instants);
    REQUIRE(lhs == rhs);
    REQUIRE(!(lhs != rhs));
    REQUIRE(!(lhs < rhs));
    REQUIRE(lhs <= rhs);
    REQUIRE(lhs >= rhs);
    REQUIRE(!(lhs > rhs));
  }
  SECTION("lhs < rhs") {
    SECTION("different sizes") {
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(3, unix_time_point(2012, 1, 2)),
      };
      TInstantSet<TestType> lhs(lhs_instants);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TInstantSet<TestType> rhs(rhs_instants);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different timestamp") {
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TInstantSet<TestType> lhs(lhs_instants);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TInstantSet<TestType> rhs(rhs_instants);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different values") {
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      };
      TInstantSet<TestType> lhs(lhs_instants);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TInstantSet<TestType> rhs(rhs_instants);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("different values, different timestamp") {
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      };
      TInstantSet<TestType> lhs(lhs_instants);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TInstantSet<TestType> rhs(rhs_instants);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
  }
}

TEMPLATE_TEST_CASE("TInstantSet duration function returns InstantSet",
                   "[tinstantset]", int, float, bool, string, Geometry) {
  set<TInstant<TestType>> s;
  TInstantSet<TestType> instant_set(s);
  REQUIRE(instant_set.duration() == TemporalDuration::InstantSet);
}

TEMPLATE_TEST_CASE("TInstantSet value functions", "[tinstantset]", int, float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
  };
  TInstantSet<TestType> instant_set(instants);

  set<Range<TestType>> expected;
  for (auto const &e : instants)
    expected.insert(Range<TestType>(e.getValue(), e.getValue(), true, true));

  REQUIRE_THAT(instant_set.getValues(), UnorderedEquals(expected));
  REQUIRE(instant_set.minValue() == 1);
  REQUIRE(instant_set.maxValue() == 4);
  REQUIRE(instant_set.startValue() == 2);
  REQUIRE(instant_set.endValue() == 3);
}

TEMPLATE_TEST_CASE("TInstantSet instant functions", "[tinstantset]", int,
                   float) {
  set<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  auto size = GENERATE(0, take(4, random(1, 6)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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

TEMPLATE_TEST_CASE("TInstantSet getTime", "[tinstantset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto instant_1 = make_unique<TInstant<TestType>>(v1, tp1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, tp2);

  auto period_1 = instant_1->period();
  auto period_2 = instant_2->period();
  set<Period> periods = {period_1, period_2};
  PeriodSet expected(periods);

  set<unique_ptr<TInstant<TestType>>> instants;
  instants.insert(move(instant_1));
  instants.insert(move(instant_2));
  TInstantSet<TestType> instant_set(instants);

  REQUIRE(instant_set.timespan() ==
          instant_set.endTimestamp() - instant_set.startTimestamp());
}

TEMPLATE_TEST_CASE("TInstantSet period and timestamp related functions",
                   "[tinstantset]", int, float) {
  set<TInstant<TestType>> instants;

  auto size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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
      TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 6)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 7)),
  };
  TInstantSet<TestType> instant_set(instants);
  Period expected = Period(unix_time_point(2012, 1, 1),
                           unix_time_point(2012, 1, 7), true, true);
  REQUIRE(instant_set.period() == expected);
}

TEMPLATE_TEST_CASE("TInstantSet timespan", "[tinstantset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto instant_1 = make_unique<TInstant<TestType>>(v1, tp1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, tp2);

  set<unique_ptr<TInstant<TestType>>> instants;
  instants.insert(move(instant_1));
  instants.insert(move(instant_2));
  TInstantSet<TestType> instant_set(instants);

  REQUIRE(instant_set.timespan() ==
          instant_set.endTimestamp() - instant_set.startTimestamp());
}

TEMPLATE_TEST_CASE("TInstantSet shift", "[tinstantset]", int, float) {
  set<unique_ptr<TInstant<TestType>>> expected_instants;
  set<unique_ptr<TInstant<TestType>>> actual_instants;
  auto size = GENERATE(take(4, random(1, 2)));
  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
    auto instant = make_unique<TInstant<TestType>>(v, t);
    actual_instants.insert(instant->shift(shift));
    auto expected_instant = make_unique<TInstant<TestType>>(v, t + shift);
    expected_instants.insert(move(expected_instant));
  }

  TInstantSet<TestType> actual(actual_instants);
  TInstantSet<TestType> expected(expected_instants);
  REQUIRE_THAT(actual.getInstants(), UnorderedEquals(expected.getInstants()));
}

TEMPLATE_TEST_CASE("TInstantSet intersection functions", "[tinstantset]", int,
                   float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(10, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(20, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(30, unix_time_point(2012, 1, 4)),
  };
  TInstantSet<TestType> iset(instants);

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 2)) == true);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 3)) == true);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 4)) == true);

    // Negative cases
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 1)) == false);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 2, 1)) == false);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 4, 1)) == false);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 1, 5)) == false);
    REQUIRE(iset.intersectsTimestamp(unix_time_point(2012, 2, 1)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)) == true);
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)) == true);
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 1, 12), unix_time_point(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, false)) == true);
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, true)) == true);

    // Negative cases
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, false)) == false);
    REQUIRE(iset.intersectsPeriod(Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_point> s = {unix_time_point(2012, 1, 2)};
    REQUIRE(iset.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5)};
    REQUIRE(iset.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time_point(2012, 1, 1)};
    REQUIRE(iset.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 2, 1)};
    REQUIRE(iset.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 4, 1), unix_time_point(2012, 2, 2)};
    REQUIRE(iset.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {Period(unix_time_point(2012, 1, 2),
                            unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(iset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(iset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    s = {Period(unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(iset.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    REQUIRE(iset.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}
