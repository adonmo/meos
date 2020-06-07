#include "../../catch.hpp"
#include "../../common/matchers.hpp"
#include "../../common/time_utils.hpp"
#include <meos/types/temporal/TInstant.hpp>

const time_t day = 24 * 60 * 60 * 1000L;

TEMPLATE_TEST_CASE("TInstant instant functions", "[tinst]", int, float) {
  TestType v = random() % 1000;
  long t = unix_time(2012, 1, 1) + 10 * 365 * (random() % day);
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
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  REQUIRE(instant.timespan() == 0);
}

TEMPLATE_TEST_CASE("TInstant period", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  Period expected(unix_time(2012, 11, 1), unix_time(2012, 11, 1), true, true);
  REQUIRE(instant.period() == expected);
}

TEMPLATE_TEST_CASE("TInstant shift", "[tinst]", int, float) {
  auto i = GENERATE(0, 1, -1, 2012, 756772544,
                    take(100, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
  TInstant<TestType> instant(i, unix_time(2012, 11, 1));
  unique_ptr<TInstant<TestType>> shifted = instant.shift(day);
  REQUIRE(shifted->getTimestamp() == unix_time(2012, 11, 2));
}
