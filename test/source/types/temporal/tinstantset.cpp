#include <sstream>
#include <string>

#include <catch2/catch.hpp>

#include <meos/types/temporal/TInstantSet.hpp>

#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"

time_t const minute = 60 * 1000L;
time_t const day = 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstantSets are constructed properly", "[tinstset]", int,
                   float) {
  SECTION("reads from istream") {
    TInstantSet<TestType> instant_set;
    stringstream ss("{    10@2012-01-01  ,      20@2012-01-02 09:40:00+0530 }");
    ss >> instant_set;
    REQUIRE(instant_set.instants().size() == 2);
    REQUIRE(instant_set.startInstant() ==
            TInstant<TestType>(10, unix_time_point(2012, 1, 1)));
    REQUIRE(instant_set.endInstant() ==
            TInstant<TestType>(20, unix_time_point(2012, 1, 2, 4, 10)));
  }

  SECTION("all constructors work") {
    unique_ptr<TInstantSet<TestType>> instant_set;
    TInstant<TestType> instant_1(10, unix_time_point(2020, 9, 10));
    TInstant<TestType> instant_2(20, unix_time_point(2019, 9, 10));

    SECTION("no strings constructor") {
      TInstant<TestType> instant_3(20,
                                   unix_time_point(2019, 9, 10)); // Duplicate!
      set<TInstant<TestType>> s = {instant_1, instant_2, instant_3};
      instant_set = make_unique<TInstantSet<TestType>>(s);
    }

    SECTION("string constructor") {
      instant_set = make_unique<TInstantSet<TestType>>(
          "{10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01}");
    }

    SECTION("set of strings constructor") {
      instant_set = make_unique<TInstantSet<TestType>>(set<string>{
          "10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"});
    }

    REQUIRE(instant_set->instants().size() == 2);

    // We gave the instants out-of-order!
    REQUIRE(instant_set->startInstant() == instant_2);
    REQUIRE(instant_set->instantN(0) == instant_2);
    REQUIRE(instant_set->endInstant() == instant_1);
    REQUIRE(instant_set->instantN(1) == instant_1);
    CHECK_THROWS(instant_set->instantN(2));
  }
}

TEST_CASE("TInstantSet<Geometry> constructors", "[tinstset]") {
  SECTION("without SRID") {
    TInstant<Geometry> instant(Geometry(20, 30), unix_time_point(2012, 11, 1));
    REQUIRE(instant.srid() == 0);

    std::stringstream output;
    output << instant;
    string expected = "POINT (20 30)@2012-11-01T00:00:00+0000";
    REQUIRE(output.str() == expected);
  }

  SECTION("with SRID") {
    TInstantSet<Geometry> instant_set;
    int expected_srid = 4326;
    TInstant<Geometry> expected_inst_1(Geometry(20, 30, expected_srid),
                                       unix_time_point(2012, 9, 20));
    TInstant<Geometry> expected_inst_2(Geometry(24, 32, expected_srid),
                                       unix_time_point(2012, 9, 21));
    string expected_inst_s_1 = "POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2 = "POINT (24 32)@2012-09-21T00:00:00+0000";
    string expected_inst_s_1_s =
        "SRID=4326;POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2_s =
        "SRID=4326;POINT (24 32)@2012-09-21T00:00:00+0000";
    string expected_inst_s_1_ds =
        "SRID=5676;POINT (20 30)@2012-09-20T00:00:00+0000";
    string expected_inst_s_2_ds =
        "SRID=5676;POINT (24 32)@2012-09-21T00:00:00+0000";
    set<TInstant<Geometry>> inst_set = {expected_inst_1, expected_inst_2};
    set<string> inst_s_set = {expected_inst_s_1, expected_inst_s_2};
    set<string> inst_s_set_s = {expected_inst_s_1_s, expected_inst_s_2_s};
    set<string> inst_s_set_ds = {expected_inst_s_1_ds, expected_inst_s_2_ds};

    string expected_prefix =
        expected_srid != 0 ? "SRID=" + to_string(expected_srid) + ";" : "";
    string expected_str_without_srid =
        "{" + expected_inst_s_1 + ", " + expected_inst_s_2 + "}";
    string expected_str = expected_prefix + expected_str_without_srid;
    string expected_str_diff_srid = "SRID=5676;" + expected_str_without_srid;

    SECTION("no strings constructor") {
      instant_set = TInstantSet<Geometry>(inst_set, expected_srid);
    }

    SECTION("set of strings constructor") {
      SECTION("with SRID int") {
        instant_set = TInstantSet<Geometry>(inst_s_set, expected_srid);
      }

      SECTION("with SRID in geom only") {
        instant_set = TInstantSet<Geometry>(inst_s_set_s, 0);
      }

      SECTION("with SRID int and SRID in geom both") {
        instant_set = TInstantSet<Geometry>(inst_s_set_s, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TInstantSet<Geometry>{inst_s_set_ds, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    SECTION("one string constructor") {
      SECTION("with SRID int") {
        instant_set =
            TInstantSet<Geometry>(expected_str_without_srid, expected_srid);
      }

      SECTION("with SRID in geom only") {
        instant_set = TInstantSet<Geometry>(expected_str, 0);
      }

      SECTION("with SRID int and SRID in geom both") {
        instant_set = TInstantSet<Geometry>(expected_str, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS(
            (TInstantSet<Geometry>{expected_str_diff_srid, expected_srid}),
            std::invalid_argument);
        return;
      }
    }

    REQUIRE(Geometry(20, 30, expected_srid) < Geometry(24, 32, expected_srid));
    REQUIRE(expected_inst_1 < expected_inst_2);
    REQUIRE(instant_set.numInstants() == 2);
    REQUIRE(instant_set.startInstant() == expected_inst_1);
    REQUIRE(instant_set.endInstant() == expected_inst_2);
    REQUIRE(instant_set.srid() == expected_srid);

    std::stringstream output;
    output << instant_set;
    REQUIRE(output.str() == expected_str);
  }
}

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
  TInstantSet<TestType> instant_set;
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
  REQUIRE(instant_set.valueN(0) == 2);
  REQUIRE(instant_set.endValue() == 3);
  REQUIRE(instant_set.valueN(3) == 3);
  CHECK_THROWS(instant_set.valueN(4));
}

TEMPLATE_TEST_CASE("TInstantSet instant functions", "[tinstantset]", int,
                   float) {
  set<TInstant<TestType>> instants;
  set<TInstant<TestType>> expected_instants;

  size_t size = GENERATE(1, take(4, random(2, 12)));

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

TEMPLATE_TEST_CASE("TInstantSet getTime", "[tinstantset]", int, float) {
  TestType v1 = GENERATE(take(2, random(0, 1000)));
  TestType v2 = GENERATE(take(2, random(0, 1000)));
  auto t1 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  auto t2 =
      GENERATE(take(2, random(unix_time(2012, 1, 1), unix_time(2020, 1, 1))));
  time_point tp1 = std::chrono::system_clock::from_time_t(t1 / 1000L);
  time_point tp2 = std::chrono::system_clock::from_time_t(t2 / 1000L);
  auto instant_1 = TInstant<TestType>(v1, tp1);
  auto instant_2 = TInstant<TestType>(v2, tp2);

  auto period_1 = instant_1.period();
  auto period_2 = instant_2.period();
  set<Period> periods = {period_1, period_2};
  PeriodSet expected(periods);

  set<TInstant<TestType>> instants;
  instants.insert(instant_1);
  instants.insert(instant_2);
  TInstantSet<TestType> instant_set(instants);

  REQUIRE(instant_set.timespan() == duration_ms(0));
}

TEMPLATE_TEST_CASE("TInstantSet period and timestamp related functions",
                   "[tinstantset]", int, float) {
  set<TInstant<TestType>> instants;

  size_t size = GENERATE(1, take(4, random(2, 100)));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    instants.insert(instant);
  }

  TInstantSet<TestType> actual(instants);
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
  auto instant_1 = TInstant<TestType>(v1, tp1);
  auto instant_2 = TInstant<TestType>(v2, tp2);

  set<TInstant<TestType>> instants;
  instants.insert(instant_1);
  instants.insert(instant_2);
  TInstantSet<TestType> instant_set(instants);

  REQUIRE(instant_set.timespan() == duration_ms(0));
}

TEMPLATE_TEST_CASE("TInstantSet shift", "[tinstantset]", int, float) {
  set<TInstant<TestType>> expected_instants;
  set<TInstant<TestType>> actual_instants;
  size_t size = GENERATE(take(4, random(1, 2)));
  duration_ms shift(GENERATE(take(4, random(minute, day))));

  for (size_t i = 0; i < size; i++) {
    TestType v = random() % 1000;
    time_point t = std::chrono::system_clock::from_time_t(
        unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
    auto instant = TInstant<TestType>(v, t);
    actual_instants.insert(*instant.shift(shift).get());
    auto expected_instant = TInstant<TestType>(v, t + shift);
    expected_instants.insert(expected_instant);
  }

  TInstantSet<TestType> actual(actual_instants);
  TInstantSet<TestType> expected(expected_instants);
  REQUIRE(actual == expected);
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
