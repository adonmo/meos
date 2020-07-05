#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;
time_t const year = 365 * 24 * 60 * 60 * 1000L;

template <typename T> TSequence<T> getSampleSequence1() {
  set<TInstant<T>> s1{
      TInstant<T>(10, unix_time_point(2012, 1, 1)),
      TInstant<T>(20, unix_time_point(2012, 1, 2, 4, 10)),
  };
  return TSequence<T>(s1, false, true);
}

template <typename T> TSequence<T> getSampleSequence2() {
  set<TInstant<T>> s1{
      TInstant<T>(40, unix_time_point(2012, 1, 7)),
      TInstant<T>(50, unix_time_point(2012, 1, 8)),
  };
  return TSequence<T>(s1, false, true);
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
    REQUIRE(sequence_set.endSequence() == getSampleSequence2<TestType>());
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
