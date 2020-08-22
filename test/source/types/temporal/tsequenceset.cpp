#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <sstream>
#include <string>

#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"

using namespace meos;
using namespace std;

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;

template <typename T>
TSequence<T> getSampleSequence1(Interpolation interpolation = default_interp_v<T>) {
  set<TInstant<T>> instants{
      TInstant<T>(10, unix_time_point(2012, 1, 1)),
      TInstant<T>(20, unix_time_point(2012, 1, 2, 4, 10)),
  };
  return TSequence<T>(instants, false, true, interpolation);
}

template <typename T>
TSequence<T> getSampleSequence2(Interpolation interpolation = default_interp_v<T>) {
  set<TInstant<T>> instants{
      TInstant<T>(40, unix_time_point(2012, 1, 7)),
      TInstant<T>(50, unix_time_point(2012, 1, 8)),
  };
  return TSequence<T>(instants, false, true, interpolation);
}

TEMPLATE_TEST_CASE("TSequenceSets are constructed properly", "[tinstset]", int, float) {
  SECTION("reads from istream") {
    TSequenceSet<TestType> sequence_set;
    stringstream ss(
        "{(    10@2012-01-01  ,      20@2012-01-02 09:40:00+0530]"
        ",( 40@2012-01-07,50@2012-01-08]}");
    string expected
        = "{(10@2012-01-01T00:00:00+0000, 20@2012-01-02T04:10:00+0000], "
          "(40@2012-01-07T00:00:00+0000, 50@2012-01-08T00:00:00+0000]}";

    ss >> sequence_set;
    REQUIRE(sequence_set.sequences().size() == 2);
    REQUIRE(sequence_set.startSequence() == getSampleSequence1<TestType>());
    REQUIRE(sequence_set.sequenceN(0) == getSampleSequence1<TestType>());
    REQUIRE(sequence_set.endSequence() == getSampleSequence2<TestType>());
    REQUIRE(sequence_set.sequenceN(1) == getSampleSequence2<TestType>());
    CHECK_THROWS(sequence_set.sequenceN(2));

    std::stringstream output;
    output << sequence_set;
    REQUIRE(output.str() == expected);
  }

  SECTION("all constructors work") {
    TSequenceSet<TestType> sequence_set;
    TSequence<TestType> sequence_1 = getSampleSequence2<TestType>();
    TSequence<TestType> sequence_2 = getSampleSequence1<TestType>();
    Interpolation expected_interp = default_interp_v<TestType>;

    SECTION("no strings constructor") {
      TSequence<TestType> sequence_3 = getSampleSequence1<TestType>();  // Duplicate!
      set<TSequence<TestType>> s = {sequence_1, sequence_2, sequence_3};
      sequence_set = TSequenceSet<TestType>(s);
    }

    SECTION("set of strings constructor") {
      sequence_set = TSequenceSet<TestType>(set<string>{
          "(10@2012-01-01, 20@2012-01-02 09:40:00+0530]", "(40@2012-01-07, 50@2012-01-08]"});
    }

    SECTION("string constructor") {
      sequence_set = TSequenceSet<TestType>(
          "{(10@2012-01-01, 20@2012-01-02 09:40:00+0530], "
          "(40@2012-01-07, 50@2012-01-08]}");
    }

    SECTION("with interpolation specified") {
      expected_interp = Interpolation::Stepwise;

      SECTION("no strings constructor") {
        TSequence<TestType> sequence_3
            = getSampleSequence1<TestType>(Interpolation::Stepwise);  // Duplicate!
        set<TSequence<TestType>> s
            = {getSampleSequence2<TestType>(Interpolation::Stepwise),
               getSampleSequence1<TestType>(Interpolation::Stepwise), sequence_3};
        sequence_set = TSequenceSet<TestType>(s, Interpolation::Stepwise);
      }

      SECTION("set of strings constructor") {
        sequence_set = TSequenceSet<TestType>(
            set<string>{"Interp=Stepwise;(10@2012-01-01, 20@2012-01-02 09:40:00+0530]",
                        "Interp=Stepwise;(40@2012-01-07, 50@2012-01-08]"},
            Interpolation::Stepwise);
      }

      SECTION("string constructor") {
        sequence_set = TSequenceSet<TestType>(
            "Interp=Stepwise;{(10@2012-01-01, 20@2012-01-02 09:40:00+0530], "
            "(40@2012-01-07, 50@2012-01-08]}");
      }
    }

    REQUIRE(sequence_set.sequences().size() == 2);

    // We gave the sequences out-of-order!
    REQUIRE(sequence_set.startSequence() == sequence_2.with_interp(expected_interp));
    REQUIRE(sequence_set.endSequence() == sequence_1.with_interp(expected_interp));

    REQUIRE(sequence_set.interpolation() == expected_interp);

    stringstream output;
    output << sequence_set;
    string expected_prefix = is_floating_point<TestType>::value
                                     && sequence_set.interpolation() == Interpolation::Stepwise
                                 ? "Interp=Stepwise;"
                                 : "";
    string expected =
        expected_prefix +
        "{(10@2012-01-01T00:00:00+0000, 20@2012-01-02T04:10:00+0000], "
        "(40@2012-01-07T00:00:00+0000, 50@2012-01-08T00:00:00+0000]}";
    REQUIRE(output.str() == expected);
  }
}

auto getSampleSequenceGeom1(int srid = 0) {
  set<TInstant<GeomPoint>> instants{
      TInstant<GeomPoint>(GeomPoint(10, 12), unix_time_point(2012, 1, 1)),
      TInstant<GeomPoint>(GeomPoint(20, 22), unix_time_point(2012, 1, 2, 4, 10)),
  };
  return TSequence<GeomPoint>(instants, false, true, srid);
}

auto getSampleSequenceGeom2(int srid = 0) {
  set<TInstant<GeomPoint>> instants{
      TInstant<GeomPoint>(GeomPoint(40, 42), unix_time_point(2012, 1, 7)),
      TInstant<GeomPoint>(GeomPoint(50, 52), unix_time_point(2012, 1, 8)),
  };
  return TSequence<GeomPoint>(instants, false, true, srid);
}

TEST_CASE("TSequenceSet<GeomPoint> constructors", "[tsequenceset]") {
  TGeomPointSeqSet seqset;
  int expected_srid = 4326;
  TSequence<GeomPoint> expected_seq_1 = getSampleSequenceGeom1(expected_srid);
  TSequence<GeomPoint> expected_seq_2 = getSampleSequenceGeom2(expected_srid);
  // Setup sequence strings without SRID
  std::stringstream output;
  output.str("");
  output << getSampleSequenceGeom1(0);
  string expected_seq_s_1 = output.str();

  output.str("");
  output << getSampleSequenceGeom2(0);
  string expected_seq_s_2 = output.str();

  // Setup sequence strings with 4326 SRID
  output.str("");
  output << expected_seq_1;
  string expected_seq_s_1_s = output.str();

  output.str("");
  output << expected_seq_2;
  string expected_seq_s_2_s = output.str();

  // Setup sequence strings with 5676 SRID
  output.str("");
  output << getSampleSequenceGeom1(5676);
  string expected_seq_s_1_ds = output.str();

  output.str("");
  output << getSampleSequenceGeom2(5676);
  string expected_seq_s_2_ds = output.str();

  set<TSequence<GeomPoint>> ss = {expected_seq_1, expected_seq_2};
  set<string> seq_s_set = {expected_seq_s_1, expected_seq_s_2};
  set<string> seq_s_set_s = {expected_seq_s_1_s, expected_seq_s_2_s};
  set<string> seq_s_set_ds = {expected_seq_s_1_ds, expected_seq_s_2_ds};

  string expected_prefix = expected_srid != 0 ? "SRID=" + to_string(expected_srid) + ";" : "";
  string expected_str_without_srid = "{" + expected_seq_s_1 + ", " + expected_seq_s_2 + "}";
  string expected_str = expected_prefix + expected_str_without_srid;
  string expected_str_diff_srid = "SRID=5676;" + expected_str_without_srid;

  SECTION("without SRID") {
    seqset = TSequenceSet<GeomPoint>(seq_s_set);
    REQUIRE(seqset.srid() == 0);
    output.str("");
    output << seqset;
    REQUIRE(output.str() == expected_str_without_srid);
  }

  SECTION("with SRID") {
    SECTION("no strings constructor") { seqset = TSequenceSet<GeomPoint>(ss, expected_srid); }

    SECTION("set of strings constructor") {
      SECTION("with SRID int") { seqset = TSequenceSet<GeomPoint>(seq_s_set, expected_srid); }

      SECTION("with SRID in geom only") { seqset = TSequenceSet<GeomPoint>(seq_s_set_s, 0); }

      SECTION("with SRID int and SRID in geom both") {
        seqset = TSequenceSet<GeomPoint>(seq_s_set_s, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TSequenceSet<GeomPoint>{seq_s_set_ds, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    SECTION("one string constructor") {
      SECTION("with SRID int") {
        seqset = TSequenceSet<GeomPoint>(expected_str_without_srid, expected_srid);
      }

      SECTION("with SRID in geom only") { seqset = TSequenceSet<GeomPoint>(expected_str, 0); }

      SECTION("with SRID int and SRID in geom both") {
        seqset = TSequenceSet<GeomPoint>(expected_str, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TSequenceSet<GeomPoint>{expected_str_diff_srid, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    REQUIRE(seqset.numSequences() == 2);
    REQUIRE(seqset.startSequence() == expected_seq_1);
    REQUIRE(seqset.endSequence() == expected_seq_2);
    REQUIRE(seqset.srid() == expected_srid);

    output.str("");
    output << seqset;
    REQUIRE(output.str() == expected_str);
  }
}

TEMPLATE_TEST_CASE("TSequenceSet comparision operators", "[tinstset]", int, float) {
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

TEMPLATE_TEST_CASE("TSequenceSet duration function returns SequenceSet", "[tsequenceset]", int,
                   float, bool, string, GeomPoint) {
  TSequenceSet<TestType> sequence_set;
  REQUIRE(sequence_set.duration() == TemporalDuration::SequenceSet);
}

TEMPLATE_TEST_CASE("TSequenceSet value functions", "[tsequenceset]", int, float) {
  auto sequences = set<TSequence<TestType>>{
      getSampleSequence1<TestType>(),
      getSampleSequence2<TestType>(),
  };
  TSequenceSet<TestType> sequence_set(sequences);

  set<Range<TestType>> expected;
  for (auto const &e : sequences)
    expected.insert(Range<TestType>(e.minValue(), e.maxValue(), e.lower_inc(), e.upper_inc()));

  REQUIRE_THAT(sequence_set.getValues(), UnorderedEquals(expected));
  REQUIRE(sequence_set.minValue() == 10);
  REQUIRE(sequence_set.maxValue() == 50);
  REQUIRE(sequence_set.startValue() == 10);
  REQUIRE(sequence_set.endValue() == 50);
}

TEMPLATE_TEST_CASE("TSequenceSet getTime and timespan", "[tsequenceset]", int, float) {
  set<TSequence<TestType>> sequences;
  size_t size = GENERATE(1, take(4, random(2, 24)));
  set<TInstant<TestType>> all_expected_instants;
  set<time_point> all_expected_timestamps;
  duration_ms expected_timespan(0);

  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    set<TInstant<TestType>> instants;
    size_t seq_size = 2 + random() % 9;

    for (size_t j = 0; j < seq_size; j++) {
      TestType v = random() % 1000;
      time_point t = std::chrono::system_clock::from_time_t(unix_time(2012, 1, 1)
                                                            + 10 * 365 * (random() % day));
      auto instant = TInstant<TestType>(v, t);
      instants.insert(instant);
      all_expected_instants.insert(instant);
      all_expected_timestamps.insert(t);
    }
    TSequence<TestType> sequence(instants);
    expected_timespan += sequence.timespan();
    sequences.insert(sequence);
  }

  TSequenceSet<TestType> actual(sequences);

  SECTION("getTime") {
    set<Period> periods = {};
    for (auto const &e : sequences) periods.insert(e.period());
    PeriodSet expected(periods);

    REQUIRE(actual.getTime() == expected);
  }

  SECTION("timespan") { REQUIRE(actual.timespan() == expected_timespan); }

  SECTION("sequence functions") {
    REQUIRE(actual.numSequences() == sequences.size());
    REQUIRE_THAT(actual.sequences(), UnorderedEquals(sequences));
    if (size > 0) {
      REQUIRE(actual.startSequence() == *sequences.begin());
      REQUIRE(actual.endSequence() == *sequences.rbegin());
    } else {
      CHECK_THROWS(actual.startSequence());
      CHECK_THROWS(actual.endSequence());
    }
  }

  SECTION("instant functions") {
    size_t sz = all_expected_instants.size();
    REQUIRE(actual.numInstants() == all_expected_instants.size());
    REQUIRE_THAT(actual.instants(), UnorderedEquals(all_expected_instants));
    if (sz > 0) {
      REQUIRE(actual.startInstant() == *all_expected_instants.begin());
      REQUIRE(actual.instantN(0) == *all_expected_instants.begin());
      REQUIRE(actual.endInstant() == *all_expected_instants.rbegin());
      REQUIRE(actual.instantN(sz - 1) == *all_expected_instants.rbegin());
      CHECK_THROWS(actual.instantN(sz));
    } else {
      CHECK_THROWS(actual.startInstant());
      CHECK_THROWS(actual.instantN(0));
      CHECK_THROWS(actual.endInstant());
      CHECK_THROWS(actual.instantN(1));
    }
  }

  SECTION("period and timestamp related functions") {
    size_t sz = all_expected_timestamps.size();
    REQUIRE(actual.numTimestamps() == all_expected_timestamps.size());
    REQUIRE_THAT(actual.timestamps(), UnorderedEquals(all_expected_timestamps));
    if (sz > 0) {
      Period p = actual.period();
      REQUIRE(p.lower() == actual.startTimestamp());
      REQUIRE(p.lower() == actual.timestampN(0));
      REQUIRE(p.upper() == actual.endTimestamp());
      REQUIRE(p.upper() == actual.timestampN(sz - 1));
      CHECK_THROWS(actual.timestampN(sz));
      REQUIRE(p.lower_inc() == true);
      REQUIRE(p.upper_inc() == true);
    } else {
      CHECK_THROWS(actual.period());
    }
  }

  SECTION("shift") {
    set<TSequence<TestType>> shifted_sequences;
    for (auto const &e : sequences) shifted_sequences.insert(*e.shift(shift).get());
    TSequenceSet<TestType> expected(shifted_sequences);
    REQUIRE(*actual.shift(shift).get() == expected);
  }
}

TEMPLATE_TEST_CASE("TSequenceSet.period() - gaps are ignored", "[tsequenceset]", int, float) {
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
  Period expected = Period(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 7), true, true);
  REQUIRE(sequence_set.period() == expected);
}

TEMPLATE_TEST_CASE("TSequenceSet intersection functions", "[tsequenceset]", int, float) {
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
    set<Period> s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)};
    s = {Period(unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5), true, true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)};
    REQUIRE(sset.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}

TEST_CASE("interpolation is maintained in sequences", "[tsequenceset]") {
  TFloatSeqSet seqset;

  const std::string EXP_TSEQSET
      = "Interp=Stepwise;{[10.0@2019-09-01 00:00:00+01], [20.0@2019-09-02 00:00:00+01, "
        "10.0@2019-09-03 00:00:00+01]}";

  std::tie(seqset) = GENERATE_COPY(table<TFloatSeqSet>({
      // String constructor
      {TFloatSeqSet(EXP_TSEQSET)},
      // Copy constructor
      {TFloatSeqSet(TFloatSeqSet(EXP_TSEQSET))},
      // Construct via deserializer
      {*(new Deserializer<float>(EXP_TSEQSET))->nextTSequenceSet().release()},
  }));

  REQUIRE(seqset.interpolation() == Interpolation::Stepwise);
  for (auto seq : seqset.sequences()) {
    REQUIRE(seq.interpolation() == Interpolation::Stepwise);
  }
}
