#include <catch2/catch.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <sstream>
#include <string>
#include <type_traits>

#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TSequences are constructed and serialized properly", "[tsequence]", int,
                   float) {
  SECTION("reads from istream") {
    TSequence<TestType> seq;
    stringstream ss("(    10@2012-01-01  ,      20@2012-01-02 09:40:00+0530 )");
    string expected = "(10@2012-01-01T00:00:00+0000, 20@2012-01-02T04:10:00+0000)";

    ss >> seq;
    REQUIRE(seq.instants().size() == 2);
    REQUIRE(seq.startInstant() == TInstant<TestType>(10, unix_time_point(2012, 1, 1)));
    REQUIRE(seq.endInstant() == TInstant<TestType>(20, unix_time_point(2012, 1, 2, 4, 10)));
    REQUIRE(seq.lower_inc() == false);
    REQUIRE(seq.upper_inc() == false);

    std::stringstream output;
    output << seq;
    REQUIRE(output.str() == expected);
  }

  SECTION("all constructors work") {
    TSequence<TestType> seq;
    TInstant<TestType> instant_1(10, unix_time_point(2020, 9, 10));
    TInstant<TestType> instant_2(20, unix_time_point(2019, 9, 10));
    Interpolation expected_interp = default_interp_v<TestType>;

    SECTION("no strings constructor") {
      TInstant<TestType> instant_3(20, unix_time_point(2019, 9, 10));  // Duplicate!
      set<TInstant<TestType>> v = {instant_1, instant_2, instant_3};
      seq = TSequence<TestType>(v, false, true);
    }

    SECTION("set of strings constructor") {
      seq = TSequence<TestType>(
          set<string>{"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, false, true);
    }

    SECTION("string constructor") {
      seq = TSequence<TestType>("(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]");
    }

    SECTION("with interpolation specified") {
      expected_interp = Interpolation::Stepwise;

      SECTION("no strings constructor") {
        TInstant<TestType> instant_3(20, unix_time_point(2019, 9, 10));  // Duplicate!
        set<TInstant<TestType>> v = {instant_1, instant_2, instant_3};
        seq = TSequence<TestType>(v, false, true, Interpolation::Stepwise);
      }

      SECTION("set of strings constructor") {
        seq = TSequence<TestType>(
            set<string>{"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, false, true,
            Interpolation::Stepwise);
      }

      SECTION("string constructor") {
        seq = TSequence<TestType>(
            "Interp=Stepwise;(10@2020-09-10 01:00:00+01, "
            "20@2019-09-10 01:00:00+01]");
      }
    }

    REQUIRE(seq.instants().size() == 2);

    // We gave the instants out-of-order!
    REQUIRE(seq.startInstant() == instant_2);
    REQUIRE(seq.instantN(0) == instant_2);
    REQUIRE(seq.endInstant() == instant_1);
    REQUIRE(seq.instantN(1) == instant_1);
    CHECK_THROWS(seq.instantN(2));

    REQUIRE(seq.lower_inc() == false);
    REQUIRE(seq.upper_inc() == true);
    REQUIRE(seq.interpolation() == expected_interp);

    stringstream output;
    output << seq;
    string expected_prefix
        = is_floating_point<TestType>::value && seq.interpolation() == Interpolation::Stepwise
              ? "Interp=Stepwise;"
              : "";
    string expected
        = expected_prefix + "(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]";
    REQUIRE(output.str() == expected);
  }
}

TEST_CASE("TSequence<Geometry> constructors", "[tsequence]") {
  SECTION("without SRID") {
    TInstant<Geometry> instant(Geometry(20, 30), unix_time_point(2012, 11, 1));
    REQUIRE(instant.srid() == 0);

    std::stringstream output;
    output << instant;
    string expected = "POINT (20 30)@2012-11-01T00:00:00+0000";
    REQUIRE(output.str() == expected);
  }

  SECTION("with SRID") {
    TSequence<Geometry> seq;
    int expected_srid = 4326;
    TInstant<Geometry> expected_inst_1(Geometry(20, 30, expected_srid),
                                       unix_time_point(2012, 9, 20));
    TInstant<Geometry> expected_inst_2(Geometry(24, 32, expected_srid),
                                       unix_time_point(2012, 9, 21));
    string expected_inst_s_1 = "POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2 = "POINT (24 32)@2012-09-21T00:00:00+0000";
    string expected_inst_s_1_s = "SRID=4326;POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2_s = "SRID=4326;POINT (24 32)@2012-09-21T00:00:00+0000";
    string expected_inst_s_1_ds = "SRID=5676;POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2_ds = "SRID=5676;POINT (24 32)@2012-09-21T00:00:00+0000";
    set<TInstant<Geometry>> inst_set = {expected_inst_1, expected_inst_2};
    set<string> inst_s_set = {expected_inst_s_1, expected_inst_s_2};
    set<string> inst_s_set_s = {expected_inst_s_1_s, expected_inst_s_2_s};
    set<string> inst_s_set_ds = {expected_inst_s_1_ds, expected_inst_s_2_ds};

    string expected_prefix = expected_srid != 0 ? "SRID=" + to_string(expected_srid) + ";" : "";
    string expected_str_without_srid = "[" + expected_inst_s_1 + ", " + expected_inst_s_2 + ")";
    string expected_str = expected_prefix + expected_str_without_srid;
    string expected_str_diff_srid = "SRID=5676;" + expected_str_without_srid;

    SECTION("no strings constructor") {
      seq = TSequence<Geometry>(inst_set, true, false, expected_srid);
    }

    SECTION("set of strings constructor") {
      SECTION("with SRID int") {
        seq = TSequence<Geometry>(inst_s_set, true, false, expected_srid);
      }

      SECTION("with SRID in geom only") { seq = TSequence<Geometry>(inst_s_set_s, true, false, 0); }

      SECTION("with SRID int and SRID in geom both") {
        seq = TSequence<Geometry>(inst_s_set_s, true, false, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TSequence<Geometry>{inst_s_set_ds, true, false, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    SECTION("one string constructor") {
      SECTION("with SRID int") {
        seq = TSequence<Geometry>(expected_str_without_srid, expected_srid);
      }

      SECTION("with SRID in geom only") { seq = TSequence<Geometry>(expected_str, 0); }

      SECTION("with SRID int and SRID in geom both") {
        seq = TSequence<Geometry>(expected_str, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TSequence<Geometry>{expected_str_diff_srid, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    REQUIRE(seq.numInstants() == 2);
    REQUIRE(seq.startInstant() == expected_inst_1);
    REQUIRE(seq.endInstant() == expected_inst_2);
    REQUIRE(seq.lower_inc() == true);
    REQUIRE(seq.upper_inc() == false);
    REQUIRE(seq.srid() == expected_srid);

    std::stringstream output;
    output << seq;
    REQUIRE(output.str() == expected_str);
  }
}

TEMPLATE_TEST_CASE("TSequence comparision operators", "[tsequence]", int, float) {
  SECTION("lhs == rhs") {
    set<TInstant<TestType>> lhs_instants = {
        TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
        TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
        TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
        TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
    };
    TSequence<TestType> lhs(lhs_instants, true, false);
    set<TInstant<TestType>> rhs_instants = {
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
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
          TInstant<TestType>(3, unix_time_point(2012, 1, 3)),
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
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
      };
      TSequence<TestType> lhs(lhs_instants, true, true);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
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
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 6)),
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
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(2, unix_time_point(2012, 1, 5)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
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
      set<TInstant<TestType>> lhs_instants = {
          TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
          TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      };
      TSequence<TestType> lhs(lhs_instants, true, false);
      set<TInstant<TestType>> rhs_instants = {
          TInstant<TestType>(4, unix_time_point(2012, 1, 4)),
          TInstant<TestType>(5, unix_time_point(2012, 1, 5)),
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

TEMPLATE_TEST_CASE("TSequence duration function returns Sequence", "[tsequence]", int, float, bool,
                   string, Geometry) {
  TSequence<TestType> sequence;
  REQUIRE(sequence.duration() == TemporalDuration::Sequence);
}

TEMPLATE_TEST_CASE("TSequence value functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);

  set<TInstant<TestType>> instants = {
      TInstant<TestType>(2, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(1, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 4)),
  };
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  set<Range<TestType>> expected
      = {Range<TestType>(sequence.minValue(), sequence.maxValue(), lower_inc, upper_inc)};

  REQUIRE_THAT(sequence.getValues(), UnorderedEquals(expected));
  REQUIRE(sequence.minValue() == 1);
  REQUIRE(sequence.maxValue() == 4);
  REQUIRE(sequence.startValue() == 2);
  REQUIRE(sequence.valueN(0) == 2);
  REQUIRE(sequence.endValue() == 3);
  REQUIRE(sequence.valueN(3) == 3);
  CHECK_THROWS(sequence.valueN(4));
}

TEMPLATE_TEST_CASE("TSequence instant functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  set<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  // TODO test size = 1
  size_t size = GENERATE(take(4, random(2, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(unix_time(2012, 1, 1)
                                                          + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
    expected_instants.insert(instant);
  }

  TSequence<TestType> actual(instants, lower_inc, upper_inc);
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

TEMPLATE_TEST_CASE("TSequence getTime", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 = GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 = GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto instant_1 = TInstant<TestType>(v1, tp1);
  auto instant_2 = TInstant<TestType>(v2, tp2);

  set<TInstant<TestType>> instants;
  instants.insert(instant_1);
  instants.insert(instant_2);
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  auto period = sequence.period();
  set<Period> periods = {period};
  PeriodSet expected(periods);

  REQUIRE(sequence.getTime() == expected);
}

TEMPLATE_TEST_CASE("TSequence period and timestamp related functions", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  set<TInstant<TestType>> instants;

  // TODO test size = 1
  size_t size = GENERATE(take(4, random(2, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(unix_time(2012, 1, 1)
                                                          + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
  }

  TSequence<TestType> actual(instants, lower_inc, upper_inc);
  REQUIRE_THAT(actual.instants(), UnorderedEquals(instants));
  if (size > 0) {
    Period p = actual.period();
    REQUIRE(p.lower() == actual.startTimestamp());
    REQUIRE(p.lower() == actual.timestampN(0));
    REQUIRE(p.upper() == actual.endTimestamp());
    REQUIRE(p.upper() == actual.timestampN(size - 1));
    CHECK_THROWS(actual.timestampN(size));
    REQUIRE(p.lower_inc() == lower_inc);
    REQUIRE(p.upper_inc() == upper_inc);
  } else {
    CHECK_THROWS(actual.period());
  }
}

TEMPLATE_TEST_CASE("TSequence.period() - gaps are ignored", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(1, unix_time_point(2012, 1, 1)),
      TInstant<TestType>(2, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(3, unix_time_point(2012, 1, 6)),
      TInstant<TestType>(4, unix_time_point(2012, 1, 7)),
  };
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);
  Period expected
      = Period(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 7), lower_inc, upper_inc);
  REQUIRE(sequence.period() == expected);
}

TEMPLATE_TEST_CASE("TSequence timespan", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 = GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 = GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2);
  auto instant_1 = TInstant<TestType>(v1, tp1);
  auto instant_2 = TInstant<TestType>(v2, tp2);

  set<TInstant<TestType>> instants;
  instants.insert(instant_1);
  instants.insert(instant_2);
  TSequence<TestType> sequence(instants, lower_inc, upper_inc);

  REQUIRE(sequence.timespan()
          == std::chrono::duration_cast<duration_ms>(sequence.endTimestamp()
                                                     - sequence.startTimestamp()));
}

TEMPLATE_TEST_CASE("TSequence shift", "[tsequence]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);

  set<TInstant<TestType>> expected_instants;
  set<TInstant<TestType>> actual_instants;
  size_t size = GENERATE(take(4, random(2, 24)));
  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(unix_time(2012, 1, 1)
                                                          + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    actual_instants.insert(*instant.shift(shift).get());
    auto expected_instant = TInstant<TestType>(v, t + shift);
    expected_instants.insert(expected_instant);
  }

  TSequence<TestType> actual(actual_instants, lower_inc, upper_inc);
  TSequence<TestType> expected(expected_instants, lower_inc, upper_inc);
  REQUIRE_THAT(actual.instants(), UnorderedEquals(expected.instants()));
}

TEMPLATE_TEST_CASE("TSequence intersection functions", "[tsequence]", int, float) {
  set<TInstant<TestType>> instants = {
      TInstant<TestType>(10, unix_time_point(2012, 1, 2)),
      TInstant<TestType>(20, unix_time_point(2012, 1, 3)),
      TInstant<TestType>(30, unix_time_point(2012, 1, 4)),
  };
  TSequence<TestType> sequence(instants, true, true);

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 2)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 2, 1)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 3)) == true);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 4)) == true);

    // Negative cases
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 1)) == false);
    REQUIRE(sequence.intersectsTimestamp(unix_time_point(2012, 1, 4, 1)) == false);
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
    set<Period> s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)};
    s = {Period(unix_time_point(2012, 1, 3), unix_time_point(2012, 1, 5), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == true);

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)};
    REQUIRE(sequence.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}

TEST_CASE("TSequence<Geometry> with_srid", "[tsequence]") {
  TSequence<Geometry> g(
      "[POINT (10 12)@2011-01-01T00:00:00+0000, POINT (40 42)@2011-01-02T00:00:00+0000)");
  REQUIRE(g.srid() == 0);
  REQUIRE(g.startInstant().srid() == 0);
  REQUIRE(g.startValue().srid() == 0);
  g = g.with_srid(4326);
  REQUIRE(g.srid() == 4326);
  REQUIRE(g.startInstant().srid() == 4326);
  REQUIRE(g.startValue().srid() == 4326);
}
