#include <sstream>
#include <string>

#include <catch2/catch.hpp>

#include <meos/types/range/Range.hpp>
#include <meos/types/temporal/TInstant.hpp>

#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"

time_t const day = 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstants are constructed properly", "[tinstant]", int,
                   float) {
  SECTION("reads from istream") {
    TInstant<TestType> instant;
    stringstream ss("    1 @     2012-01-02 09:40:00.789+0530 ");
    ss >> instant;
    REQUIRE(instant.getValue() == 1);
    REQUIRE(instant.getTimestamp() ==
            unix_time_point(2012, 1, 2, 4, 10, 0, 789));

    std::stringstream output;
    output << instant;
    string expected = "1@2012-01-02T04:10:00.789+0000";
    REQUIRE(output.str() == expected);
  }

  SECTION("all constructors work") {
    unique_ptr<TInstant<TestType>> instant;
    SECTION("no strings constructor") {
      instant =
          make_unique<TInstant<TestType>>(1, unix_time_point(2019, 9, 21));
    }
    SECTION("two strings constructor") {
      instant = make_unique<TInstant<TestType>>("1", "2019-09-21 01:00:00+01");
    }
    SECTION("one string constructor") {
      instant = make_unique<TInstant<TestType>>("1@2019-09-21 01:00:00+01");
    }
    SECTION("pair constructor") {
      SECTION("no strings") {
        instant = make_unique<TInstant<TestType>>(
            make_pair(1, unix_time_point(2019, 9, 21)));
      }
      SECTION("two strings") {
        instant = make_unique<TInstant<TestType>>(
            make_pair("1", "2019-09-21 01:00:00+01"));
      }
    }
    REQUIRE(instant->getValue() == 1);
    REQUIRE(instant->getTimestamp() == unix_time_point(2019, 9, 21));
  }
}

TEMPLATE_TEST_CASE("TInstant comparision operators", "[tinst]", int, float,
                   bool) {
  SECTION("lhs == rhs") {
    TInstant<TestType> lhs(1, unix_time_point(2012, 1, 1));
    TInstant<TestType> rhs(1, unix_time_point(2012, 1, 1));
    REQUIRE(lhs == rhs);
    REQUIRE(!(lhs != rhs));
    REQUIRE(!(lhs < rhs));
    REQUIRE(lhs <= rhs);
    REQUIRE(lhs >= rhs);
    REQUIRE(!(lhs > rhs));
  }
  SECTION("lhs < rhs") {
    SECTION("same values, different timestamp") {
      TInstant<TestType> lhs(1, unix_time_point(2012, 1, 1));
      TInstant<TestType> rhs(1, unix_time_point(2012, 1, 2));
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("different values, same timestamp") {
      TInstant<TestType> lhs(0, unix_time_point(2012, 1, 1));
      TInstant<TestType> rhs(1, unix_time_point(2012, 1, 1));
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
    SECTION("different values, different timestamp") {
      TInstant<TestType> lhs(0, unix_time_point(2012, 1, 1));
      TInstant<TestType> rhs(1, unix_time_point(2012, 1, 2));
      REQUIRE(!(lhs == rhs));
      REQUIRE(lhs != rhs);
      REQUIRE(lhs < rhs);
      REQUIRE(lhs <= rhs);
      REQUIRE(!(lhs >= rhs));
      REQUIRE(!(lhs > rhs));
    }
  }
}

TEMPLATE_TEST_CASE("TInstant duration function returns Instant", "[tinst]", int,
                   float, bool) {
  TInstant<TestType> instant(1, unix_time_point(2012, 1, 1));
  REQUIRE(instant.duration() == TemporalDuration::Instant);
}

TEMPLATE_TEST_CASE("TInstant value functions", "[tinst]", int, float, bool) {
  TestType value = 1;
  TInstant<TestType> instant(value, unix_time_point(2012, 1, 1));

  set<Range<TestType>> expected = {
      Range<TestType>(instant.getValue(), instant.getValue(), true, true)};

  REQUIRE_THAT(instant.getValues(), UnorderedEquals(expected));
  REQUIRE(instant.startValue() == value);
  REQUIRE(instant.endValue() == value);
  REQUIRE(instant.minValue() == value);
  REQUIRE(instant.maxValue() == value);
}

TEMPLATE_TEST_CASE("TInstant instant functions", "[tinst]", int, float) {
  TestType v = random() % 1000;
  time_point t = std::chrono::system_clock::from_time_t(
      unix_time(2012, 1, 1) + 10 * 365 * (random() % day));
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
  TInstant<TestType> instant(i, unix_time_point(2012, 11, 1));
  REQUIRE(instant.timespan() == duration_ms(0));
}

TEMPLATE_TEST_CASE("TInstant getTime", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time_point(2012, 11, 1));
  Period p(unix_time_point(2012, 11, 1), unix_time_point(2012, 11, 1), true,
           true);
  set<Period> periods = {p};
  PeriodSet expected(periods);
  REQUIRE(instant.getTime() == expected);
}

TEMPLATE_TEST_CASE("TInstant period", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time_point(2012, 11, 1));
  Period expected(unix_time_point(2012, 11, 1), unix_time_point(2012, 11, 1),
                  true, true);
  REQUIRE(instant.period() == expected);
}

TEMPLATE_TEST_CASE("TInstant shift", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time_point(2012, 11, 1));
  unique_ptr<TInstant<TestType>> shifted = instant.shift(duration_ms(day));
  REQUIRE(shifted->getTimestamp() == unix_time_point(2012, 11, 2));
}

TEMPLATE_TEST_CASE("TInstant intersection functions", "[tinstantset]", int,
                   float) {
  TInstant<TestType> instant(10, unix_time_point(2012, 1, 2));

  SECTION("intersectsTimestamp") {
    // Positive cases
    REQUIRE(instant.intersectsTimestamp(unix_time_point(2012, 1, 2)) == true);

    // Negative cases
    REQUIRE(instant.intersectsTimestamp(unix_time_point(2012, 1, 1)) == false);
    REQUIRE(instant.intersectsTimestamp(unix_time_point(2012, 1, 2, 1)) ==
            false);
    REQUIRE(instant.intersectsTimestamp(unix_time_point(2012, 2, 2)) == false);
  }

  // clang-format off
  SECTION("intersectsPeriod") {
    // Positive cases
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 1, 12), unix_time_point(2012, 1, 2, 12), true, true)) == true);
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), true, false)) == true);

    // Negative cases
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, true)) == false);
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 3), false, false)) == false);
    REQUIRE(instant.intersectsPeriod(Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true, true)) == false);
  }
  // clang-format on

  SECTION("intersectsTimestampSet") {
    // Positive cases
    set<time_point> s = {unix_time_point(2012, 1, 2)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == true);
    s = {unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == true);

    // Negative cases
    s = {unix_time_point(2012, 1, 1)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 2, 1)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
    s = {unix_time_point(2012, 1, 4, 1), unix_time_point(2012, 2, 2)};
    REQUIRE(instant.intersectsTimestampSet(TimestampSet(s)) == false);
  }

  SECTION("intersectsPeriodSet") {
    // Positive cases
    set<Period> s = {Period(unix_time_point(2012, 1, 2),
                            unix_time_point(2012, 1, 3), true, true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == true);
    s = {Period(unix_time_point(2012, 1, 2), unix_time_point(2012, 1, 5), true,
                true),
         Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};

    // Negative cases
    s = {Period(unix_time_point(2012, 2, 2), unix_time_point(2012, 2, 3), true,
                true)};
    REQUIRE(instant.intersectsPeriodSet(PeriodSet(s)) == false);
  }
}

TEST_CASE("TInstant<Geometry>", "[tinst]") {
  SECTION("without SRID") {
    TInstant<Geometry> instant(Geometry(20, 30), unix_time_point(2012, 11, 1));
    REQUIRE(instant.srid() == 0);

    std::stringstream output;
    output << instant;
    string expected = "POINT (20 30)@2012-11-01T00:00:00+0000";
    REQUIRE(output.str() == expected);
  }

  SECTION("with SRID") {
    TInstant<Geometry> instant;
    int expected_srid = 4326;
    Geometry expected_geom = Geometry(20, 30, expected_srid);
    string expected_geom_s = "SRID=4326;POINT (20 30)";
    time_point expected_time = unix_time_point(2012, 11, 1);
    string expected_time_s = "2012-11-01T00:00:00+0000";

    SECTION("no strings constructor") {
      instant = TInstant<Geometry>(expected_geom, expected_time, expected_srid);
    }

    SECTION("two strings constructor") {
      SECTION("with SRID int") {
        instant =
            TInstant<Geometry>("POINT (20 30)", expected_time_s, expected_srid);
      }

      SECTION("with SRID in geom only") {
        instant = TInstant<Geometry>(expected_geom_s, expected_time_s, 0);
      }

      SECTION("with SRID int and SRID in geom both") {
        instant =
            TInstant<Geometry>(expected_geom_s, expected_time_s, expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS((TInstant<Geometry>{"SRID=5676;POINT (20 30)",
                                              expected_time_s, expected_srid}),
                          std::invalid_argument);
        return;
      }
    }

    SECTION("one string constructor") {
      SECTION("with SRID int") {
        instant = TInstant<Geometry>("POINT (20 30)@" + expected_time_s,
                                     expected_srid);
      }

      SECTION("with SRID in geom only") {
        instant =
            TInstant<Geometry>(expected_geom_s + "@" + expected_time_s, 0);
      }

      SECTION("with SRID int and SRID in geom both") {
        instant = TInstant<Geometry>(expected_geom_s + "@" + expected_time_s,
                                     expected_srid);
      }

      SECTION("conflicting SRIDs") {
        REQUIRE_THROWS_AS(
            (TInstant<Geometry>{expected_geom_s + "@" + expected_time_s, 5676}),
            std::invalid_argument);
        return;
      }
    }

    SECTION("pair constructor") {
      SECTION("no strings") {
        instant = TInstant<Geometry>(make_pair(expected_geom, expected_time),
                                     expected_srid);
      }
      SECTION("two strings") {
        instant = TInstant<Geometry>(
            make_pair(expected_geom_s, expected_time_s), expected_srid);
      }
    }

    REQUIRE(instant.getValue() == expected_geom);
    REQUIRE(instant.getTimestamp() == expected_time);
    REQUIRE(instant.srid() == expected_srid);

    std::stringstream output;
    output << instant;
    string expected = expected_geom_s + "@" + expected_time_s;
    REQUIRE(output.str() == expected);
  }

  // TODO
  // Testing for this would be more involved, probably we need to use SFINAE
  //
  // SECTION("Non geometries cannot be constructed with SRIDs") {
  //
  // }
}
