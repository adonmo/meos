#include "../../catch.hpp"
#include "../../common/time_utils.hpp"
#include "../../common/utils.hpp"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>

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
  const time_t day = 24 * 60 * 60 * 1000L;
  auto shifted = instant.shift(day);
  REQUIRE(shifted->getTimestamp() == unix_time(2012, 11, 2));
}
