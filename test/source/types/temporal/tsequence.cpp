#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/temporal/TSequence.hpp>

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TSequences are constructed properly", "[tinstset]", int,
                   float) {
  SECTION("reads from istream") {
    TSequence<TestType> seq;
    stringstream ss("(    10@2012-01-01  ,      20@2012-01-02 09:40:00+0530 )");
    ss >> seq;
    REQUIRE(seq.instants().size() == 2);
    REQUIRE(seq.startInstant() ==
            TInstant<TestType>(10, unix_time_point(2012, 1, 1)));
    REQUIRE(seq.endInstant() ==
            TInstant<TestType>(20, unix_time_point(2012, 1, 2, 4, 10)));
    REQUIRE(seq.lower_inc() == false);
    REQUIRE(seq.upper_inc() == false);
  }

  SECTION("all constructors work") {
    unique_ptr<TSequence<TestType>> seq;
    TInstant<TestType> instant_1(10, unix_time_point(2020, 9, 10));
    TInstant<TestType> instant_2(20, unix_time_point(2019, 9, 10));

    SECTION("no strings constructor") {
      TInstant<TestType> instant_3(20,
                                   unix_time_point(2019, 9, 10)); // Duplicate!
      vector<TInstant<TestType>> v = {instant_1, instant_2, instant_3};
      seq = make_unique<TSequence<TestType>>(v, false, true);
    }

    SECTION("string constructor") {
      seq = make_unique<TSequence<TestType>>(
          "(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]");
    }

    SECTION("vector of strings constructor") {
      seq = make_unique<TSequence<TestType>>(
          vector<string>{"10@2020-09-10 01:00:00+01",
                         "20@2019-09-10 01:00:00+01"},
          false, true);
    }

    REQUIRE(seq->instants().size() == 2);

    // We gave the instants out-of-order!
    REQUIRE(seq->startInstant() == instant_2);
    REQUIRE(seq->endInstant() == instant_1);

    REQUIRE(seq->lower_inc() == false);
    REQUIRE(seq->upper_inc() == true);
  }
}

TEMPLATE_TEST_CASE("TSequence comparision operators", "[tsequence]", int,
                   float) {
  SECTION("lhs == rhs") {
    vector<TInstant<TestType>> lhs_instants = {
        TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
        TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
        TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
        TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
    };
    TSequence<TestType> lhs(lhs_instants, true, false);
    vector<TInstant<TestType>> rhs_instants = {
        TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
        TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
        TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
        TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
    };
    TSequence<TestType> rhs(rhs_instants, true, false);
    REQUIRE(lhs == rhs);
    REQUIRE(!(lhs != rhs));
    REQUIRE(!(lhs < rhs));
    REQUIRE(lhs <= rhs);
    REQUIRE(lhs >= rhs);
    REQUIRE(!(lhs > rhs));
  }
  SECTION("lhs < rhs") {
    SECTION("different sizes") {
      vector<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(3, unix_time_point(2012, 1, 2)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      vector<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TSequence<TestType> rhs(rhs_instants, true, false);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different bounds") {
      // TODO: This section could use some better tests
      vector<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> lhs(lhs_instants, true, true);
      vector<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> rhs(rhs_instants, false, true);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different timestamp") {
      vector<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      vector<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TSequence<TestType> rhs(rhs_instants, true, false);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different values") {
      vector<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      vector<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> rhs(rhs_instants, true, false);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("different values, different timestamp") {
      vector<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      vector<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TSequence<TestType> rhs(rhs_instants, true, false);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
  }
}

TEMPLATE_TEST_CASE("TSequence duration function returns Sequence",
                   "[tsequence]", int, float, bool, string, Geometry) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> v;
  TSequence<TestType> sequence(v, lower_inc, upper_inc);
  REQUIRE(sequence.duration() == TemporalDuration::Sequence);
}

TEMPLATE_TEST_CASE("TSequence value functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);

  vector<TInstant<TestType>> instants = {
      TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
  };
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  set<Range<TestType>> expected = {Range<TestType>(
      sequence.minValue(), sequence.maxValue(), lower_inc, upper_inc)};

  REQUIRE_THAT(sequence.getValues(), UnorderedEquals(expected));
  REQUIRE(sequence.minValue() == 1);
  REQUIRE(sequence.maxValue() == 4);
  REQUIRE(sequence.startValue() == 2);
  REQUIRE(sequence.endValue() == 3);
}

TEMPLATE_TEST_CASE("TSequence instant functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  vector<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  size_t size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto instant_1 = make_unique<TInstant<TestType>>(v1, tp1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, tp2);

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

  size_t size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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
      TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 6)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 7)),
  };
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);
  Period expected = Period(unix_time_point(2012, 1, 1),
                           unix_time_point(2012, 1, 7), lower_inc, upper_inc);
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
  time_point tp1 = std::chrono::system_clock::from_time_t(t1);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2);
  auto instant_1 = make_unique<TInstant<TestType>>(v1, tp1);
  auto instant_2 = make_unique<TInstant<TestType>>(v2, tp2);

  vector<unique_ptr<TInstant<TestType>>> instants;
  instants.push_back(move(instant_1));
  instants.push_back(move(instant_2));
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  REQUIRE(sequence.timespan() ==
          std::chrono::duration_cast<duration_ms>(sequence.endTimestamp() -
                                                  sequence.startTimestamp()));
}

TEMPLATE_TEST_CASE("TSequence shift", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);

  vector<unique_ptr<TInstant<TestType>>> expected_instants;
  vector<unique_ptr<TInstant<TestType>>> actual_instants;
  size_t size = GENERATE(take(4, random(1, 2)));
  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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
      TInstant<TestType>(10, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(20, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(30, unix_time_point(2012, 1, 4)),
  };
  TSequence<TestType> sequence(instants, true, true);

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 2)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 2, 1)) ==
            true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 3)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 4)) == true);

    // Negative cases
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 1)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 4, 1)) ==
            false);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 5)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 2, 1)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 1, 12), unix_time_point(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, false)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, true)) == true);
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, false)) == true);

    // Negative cases
    REQUIRE(sequence.intersectsPeriod(Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_point> s = {unix_time_point(2012, 1, 2)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time_point(2012, 1, 2, 1)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time_point(2012, 1, 1)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 4, 1), unix_time_point(2012, 2, 2)};
    REQUIRE(sequence.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {Period(unix_time_point(2012, 1, 2),
                            unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    s = {Period(unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}
