#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

template <typename T> TSequence<T> getSampleSequence1() {
  set<TInstant<T>> instants{
      TInstant<T>(10, unix_time_point(2012, 1, 1)),
      TInstant<T>(20, unix_time_point(2012, 1, 2, 4, 10)),
  };
  return TSequence<T>(instants, false, true);
}

template <typename T> TSequence<T> getSampleSequence2() {
  set<TInstant<T>> instants{
      TInstant<T>(40, unix_time_point(2012, 1, 7)),
      TInstant<T>(50, unix_time_point(2012, 1, 8)),
  };
  return TSequence<T>(instants, false, true);
}

TEMPLATE_TEST_CASE("TSequenceSets are constructed properly", "[tinstset]", int,
                   float) {
  SECTION("reads from istream") {
    TSequenceSet<TestType> sequence_set;
    stringstream ss("{(    10@2012-01-01  ,      20@2012-01-02 09:40:00+0530]"
                    ",( 40@2012-01-07,50@2012-01-08]}");
    ss >> sequence_set;

    REQUIRE(sequence_set.sequences().size() == 2);
    REQUIRE(sequence_set.startSequence() == getSampleSequence1<TestType>());
    REQUIRE(sequence_set.sequenceN(0) == getSampleSequence1<TestType>());
    REQUIRE(sequence_set.endSequence() == getSampleSequence2<TestType>());
    REQUIRE(sequence_set.sequenceN(1) == getSampleSequence2<TestType>());
    CHECK_THROWS(sequence_set.sequenceN(2));
  }

  SECTION("all constructors work") {
    unique_ptr<TSequenceSet<TestType>> sequence_set;
    TSequence<TestType> sequence_1 = getSampleSequence2<TestType>();
    TSequence<TestType> sequence_2 = getSampleSequence1<TestType>();

    SECTION("no strings constructor") {
      TSequence<TestType> sequence_3 =
          getSampleSequence1<TestType>(); // Duplicate!
      set<TSequence<TestType>> s = {sequence_1, sequence_2, sequence_3};
      sequence_set = make_unique<TSequenceSet<TestType>>(s);
    }

    SECTION("string constructor") {
      sequence_set = make_unique<TSequenceSet<TestType>>(
          "{(10@2012-01-01, 20@2012-01-02 09:40:00+0530], (40@2012-01-07, "
          "50@2012-01-08]}");
    }

    SECTION("set of strings constructor") {
      sequence_set = make_unique<TSequenceSet<TestType>>(
          set<string>{"(10@2012-01-01, 20@2012-01-02 09:40:00+0530]",
                      "(40@2012-01-07, 50@2012-01-08]"});
    }

    REQUIRE(sequence_set->sequences().size() == 2);

    // We gave the sequences out-of-order!
    REQUIRE(sequence_set->startSequence() == sequence_2);
    REQUIRE(sequence_set->endSequence() == sequence_1);
  }
}

TEMPLATE_TEST_CASE("TSequenceSet comparision operators", "[tinstset]", int,
                   float) {
  SECTION("lhs == rhs") {
    auto lhs_sequences = set<TSequence<TestType>>{
        getSampleSequence2<TestType>(),
    };
    TSequenceSet<TestType> lhs(lhs_sequences);

    set<TInstant<TestType>> rhs_instants_1{
        TInstant<TestType>(40, unix_time_point(2012, 1, 7)),
        TInstant<TestType>(50, unix_time_point(2012, 1, 8)),
    };
    auto rhs_sequences = set<TSequence<TestType>>{
        TSequence<TestType>(rhs_instants_1, false, true),
    };
    TSequenceSet<TestType> rhs(rhs_sequences);

    REQUIRE(lhs == rhs);
    REQUIRE(!(lhs != rhs));
    REQUIRE(!(lhs < rhs));
    REQUIRE(lhs <= rhs);
    REQUIRE(lhs >= rhs);
    REQUIRE(!(lhs > rhs));
  }
  SECTION("lhs < rhs") {
    SECTION("different sizes") {
      auto lhs_sequences = set<TSequence<TestType>>{
          getSampleSequence2<TestType>(),
      };
      TSequenceSet<TestType> lhs(lhs_sequences);
      auto rhs_sequences = set<TSequence<TestType>>{
          getSampleSequence1<TestType>(),
          getSampleSequence2<TestType>(),
      };
      TSequenceSet<TestType> rhs(rhs_sequences);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different timestamp") {
      auto lhs_sequences = set<TSequence<TestType>>{
          getSampleSequence2<TestType>(),
      };
      TSequenceSet<TestType> lhs(lhs_sequences);

      set<TInstant<TestType>> rhs_instants_1{
          TInstant<TestType>(40, unix_time_point(2012, 1, 7)),
          TInstant<TestType>(50, unix_time_point(2012, 1, 9)),
      };
      auto rhs_sequences = set<TSequence<TestType>>{
          TSequence<TestType>(rhs_instants_1, false, true),
      };
      TSequenceSet<TestType> rhs(rhs_sequences);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("same everything, except different values") {
      auto lhs_sequences = set<TSequence<TestType>>{
          getSampleSequence2<TestType>(),
      };
      TSequenceSet<TestType> lhs(lhs_sequences);

      set<TInstant<TestType>> rhs_instants_1{
          TInstant<TestType>(42, unix_time_point(2012, 1, 7)),
          TInstant<TestType>(50, unix_time_point(2012, 1, 8)),
      };
      auto rhs_sequences = set<TSequence<TestType>>{
          TSequence<TestType>(rhs_instants_1, false, true),
      };
      TSequenceSet<TestType> rhs(rhs_sequences);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("different values, different timestamp") {
      auto lhs_sequences = set<TSequence<TestType>>{
          getSampleSequence2<TestType>(),
      };
      TSequenceSet<TestType> lhs(lhs_sequences);

      set<TInstant<TestType>> rhs_instants_1{
          TInstant<TestType>(34, unix_time_point(2012, 1, 7, 1)),
          TInstant<TestType>(35, unix_time_point(2012, 1, 8)),
      };
      auto rhs_sequences = set<TSequence<TestType>>{
          TSequence<TestType>(rhs_instants_1, false, true),
      };
      TSequenceSet<TestType> rhs(rhs_sequences);
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
  }
}

TEMPLATE_TEST_CASE("TSequenceSet duration function returns SequenceSet",
                   "[tsequenceset]", int, float, bool, string, Geometry) {
  TSequenceSet<TestType> sequence_set;
  REQUIRE(sequence_set.duration() == TemporalDuration::SequenceSet);
}

TEMPLATE_TEST_CASE("TSequenceSet value functions", "[tsequenceset]", int,
                   float) {
  auto sequences = set<TSequence<TestType>>{
      getSampleSequence1<TestType>(),
      getSampleSequence2<TestType>(),
  };
  TSequenceSet<TestType> sequence_set(sequences);

  set<Range<TestType>> expected;
  for (auto const &e : sequences)
    expected.insert(Range<TestType>(e.minValue(), e.maxValue(), e.lower_inc(),
                                    e.upper_inc()));

  REQUIRE_THAT(sequence_set.getValues(), UnorderedEquals(expected));
  REQUIRE(sequence_set.minValue() == 10);
  REQUIRE(sequence_set.maxValue() == 50);
  REQUIRE(sequence_set.startValue() == 10);
  REQUIRE(sequence_set.endValue() == 50);
}

TEMPLATE_TEST_CASE("TSequenceSet sequence functions", "[tsequenceset]", int,
                   float) {
  set<TSequence<TestType>> sequences;
  set<TSequence<TestType>> expected_sequences;
  size_t size = GENERATE(0, take(2, random(1, 6)));

  for (size_t i = 0; i < size; i++) {
    set<TInstant<TestType>> instants;
    size_t seq_size = GENERATE(0, take(2, random(1, 6)));

    for (size_t j = 0; j < seq_size; j++) {
      TestType v = random() % 1000;
      time_point t = std::chrono::system_clock::from_time_t(
          unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
      auto instant = TInstant<TestType>(v, t);
      instants.insert(instant);
    }
    TSequence<TestType> sequence(instants);
    sequences.insert(sequence);
    expected_sequences.insert(sequence);
  }

  TSequenceSet<TestType> actual(expected_sequences);
  REQUIRE(actual.numSequences() == expected_sequences.size());
  REQUIRE_THAT(actual.sequences(), UnorderedEquals(expected_sequences));
  if (size > 0) {
    REQUIRE(actual.startSequence() == *expected_sequences.begin());
    REQUIRE(actual.endSequence() == *expected_sequences.rbegin());
  } else {
    CHECK_THROWS(actual.startSequence());
    CHECK_THROWS(actual.endSequence());
  }
}

TEMPLATE_TEST_CASE("TSequenceSet instant functions", "[tsequenceset]", int,
                   float) {
  set<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  size_t size = GENERATE(0, take(4, random(1, 6)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
    expected_instants.insert(instant);
  }

  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> actual(sequences);
  REQUIRE(actual.numInstants() == expected_instants.size());
  REQUIRE_THAT(actual.instants(), UnorderedEquals(expected_instants));
  if (size > 0) {
    REQUIRE(actual.startInstant() == *expected_instants.begin());
    REQUIRE(actual.instantN(0) == *expected_instants.begin());
    REQUIRE(actual.endInstant() == *expected_instants.rbegin());
    REQUIRE(actual.instantN(size - 1) == *expected_instants.rbegin());
    CHECK_THROWS(actual.instantN(size));
  } else {
    CHECK_THROWS(actual.startInstant());
    CHECK_THROWS(actual.instantN(0));
    CHECK_THROWS(actual.endInstant());
    CHECK_THROWS(actual.instantN(1));
  }
}

TEMPLATE_TEST_CASE("TSequenceSet getTime", "[tsequenceset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto sequence_1 = TInstant<TestType>(v1, tp1);
  auto sequence_2 = TInstant<TestType>(v2, tp2);

  auto period_1 = sequence_1.period();
  auto period_2 = sequence_2.period();
  set<Period> periods = {period_1, period_2};
  PeriodSet expected(periods);

  set<TInstant<TestType>> instants;
  instants.insert(sequence_1);
  instants.insert(sequence_2);
  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> sequence_set(sequences);

  REQUIRE(sequence_set.timespan() ==
          sequence_set.endTimestamp() - sequence_set.startTimestamp());
}

TEMPLATE_TEST_CASE("TSequenceSet period and timestamp related functions",
                   "[tsequenceset]", int, float) {
  set<TInstant<TestType>> instants;

  size_t size = GENERATE(0, take(4, random(1, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        (unix_time(2012, 1, 1) + 10 * 365 * (random() % day)) / 1000L);
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
  }

  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> actual(sequences);
  REQUIRE_THAT(actual.instants(), UnorderedEquals(instants));
  if (size > 0) {
    Period p = actual.period();
    REQUIRE(p.lower() == actual.startTimestamp());
    REQUIRE(p.lower() == actual.timestampN(0));
    REQUIRE(p.upper() == actual.endTimestamp());
    REQUIRE(p.upper() == actual.timestampN(size - 1));
    CHECK_THROWS(actual.timestampN(size));
    REQUIRE(p.lower_inc() == true);
    REQUIRE(p.upper_inc() == true);
  } else {
    CHECK_THROWS(actual.period());
  }
}

TEMPLATE_TEST_CASE("TSequenceSet.period() - gaps are ignored", "[tsequenceset]",
                   int, float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 6)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 7)),
  };
  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> sequence_set(sequences);
  Period expected = Period(unix_time_point(2012, 1, 1),
                           unix_time_point(2012, 1, 7), true, true);
  REQUIRE(sequence_set.period() == expected);
}

TEMPLATE_TEST_CASE("TSequenceSet timespan", "[tsequenceset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto sequence_1 = TInstant<TestType>(v1, tp1);
  auto sequence_2 = TInstant<TestType>(v2, tp2);

  set<TInstant<TestType>> instants;
  instants.insert(sequence_1);
  instants.insert(sequence_2);
  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> sequence_set(sequences);

  REQUIRE(sequence_set.timespan() ==
          sequence_set.endTimestamp() - sequence_set.startTimestamp());
}

TEMPLATE_TEST_CASE("TSequenceSet shift", "[tsequenceset]", int, float) {
  set<TInstant<TestType>> actual_instants;
  set<TInstant<TestType>> expected_instants;
  size_t size = GENERATE(take(4, random(1, 2)));
  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        (unix_time(2012, 1, 1) + 10 * 365 * (random() % day)) / 1000L);
    auto actual_instant = TInstant<TestType>(v, t);
    actual_instants.insert(actual_instant);
    auto expected_instant = TInstant<TestType>(v, t + shift);
    expected_instants.insert(expected_instant);
  }

  auto actual_sequences = set<TSequence<TestType>>{
      TSequence<TestType>(actual_instants),
  };
  TSequenceSet<TestType> actual(actual_sequences);

  auto expected_sequences = set<TSequence<TestType>>{
      TSequence<TestType>(expected_instants),
  };
  TSequenceSet<TestType> expected(expected_sequences);

  REQUIRE(*actual.shift(shift).get() == expected);
}

TEMPLATE_TEST_CASE("TSequenceSet intersection functions", "[tsequenceset]", int,
                   float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(10, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(20, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(30, unix_time_point(2012, 1, 4)),
  };
  auto sequences = set<TSequence<TestType>>{
      TSequence<TestType>(instants),
  };
  TSequenceSet<TestType> sset(sequences);

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 2)) == true);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 3)) == true);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 4)) == true);

    // Negative cases
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 1)) == false);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 2, 1)) == false);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 4, 1)) == false);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 1, 5)) == false);
    REQUIRE(sset.intersectsTimestamp(unix_time_point(2012, 2, 1)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)) == true);
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)) == true);
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 1, 12), unix_time_point(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, false)) == true);
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, true)) == true);

    // Negative cases
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, false)) == false);
    REQUIRE(sset.intersectsPeriod(Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_point> s = {unix_time_point(2012, 1, 2)};
    REQUIRE(sset.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5)};
    REQUIRE(sset.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time_point(2012, 1, 1)};
    REQUIRE(sset.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 2, 1)};
    REQUIRE(sset.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 4, 1), unix_time_point(2012, 2, 2)};
    REQUIRE(sset.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {Period(unix_time_point(2012, 1, 2),
                            unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    s = {Period(unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}
