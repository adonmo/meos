#include "../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>

TEMPLATE_TEST_CASE("TInstants are serialized", "[serializer][tinst]", int,
                   float) {
  Serializer<TestType> w;
  SECTION("only one value present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(100, random(numeric_limits<int>::min(),
                                       numeric_limits<int>::max())));
    TInstant<TestType> instant(i, unix_time_point(2012, 11, 1));
    REQUIRE(w.write(&instant) == w.write(i) + "@2012-11-01T00:00:00+0000");
    Temporal<TestType> *temporal = &instant;
    REQUIRE(w.write(temporal) == w.write(i) + "@2012-11-01T00:00:00+0000");
  }
}

TEMPLATE_TEST_CASE("TInstants are deserialized", "[deserializer][tinst]", int,
                   float) {
  SECTION("only one TInstant present") {
    Deserializer<TestType> r("10@2012-11-01");

    TInstant<TestType> tinst = *r.nextTInstant().release();
    REQUIRE(tinst.getValue() == 10);
    REQUIRE(tinst.getTimestamp() == unix_time_point(2012, 11, 1));

    CHECK_THROWS(r.nextTInstant());
  }

  SECTION("multiple TInstants present") {
    Deserializer<TestType> r(
        "10@2012-01-01 00:00:00+00\n12@2012-04-01 00:00:00+00");

    TInstant<TestType> tinst_1 = *r.nextTInstant().release();
    REQUIRE(tinst_1.getValue() == 10);
    REQUIRE(tinst_1.getTimestamp() == unix_time_point(2012, 1, 1));

    TInstant<TestType> tinst_2 = *r.nextTInstant().release();
    REQUIRE(tinst_2.getValue() == 12);
    REQUIRE(tinst_2.getTimestamp() == unix_time_point(2012, 4, 1));

    CHECK_THROWS(r.nextTInstant());
  }
}
