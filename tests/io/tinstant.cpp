#include "../catch.hpp"
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
    unique_ptr<Temporal<TestType>> instant =
        make_unique<TInstant<TestType>>(i, 1351728000000);
    REQUIRE(w.write(instant) == w.write(i) + "@2012-11-01T00:00:00+0000");
  }
}

TEMPLATE_TEST_CASE("TInstants are deserialized", "[deserializer][tinst]", int,
                   float) {
  SECTION("only one TInstant present") {
    Deserializer<TestType> r("10@2012-11-01");

    TInstant<TestType> tinst = *r.nextTInstant().release();
    REQUIRE(tinst.getValue() == 10);
    REQUIRE(tinst.getT() == 1351728000000);

    CHECK_THROWS(r.nextTInstant());
  }

  SECTION("multiple TInstants present") {
    Deserializer<TestType> r(
        "10@2012-01-01 00:00:00+00\n12@2012-04-01 00:00:00+00");

    TInstant<TestType> tinst = *r.nextTInstant().release();
    REQUIRE(tinst.getValue() == 10);
    REQUIRE(tinst.getT() == 1325376000000);

    tinst = *r.nextTInstant().release();
    REQUIRE(tinst.getValue() == 12);
    REQUIRE(tinst.getT() == 1333238400000);

    CHECK_THROWS(r.nextTInstant());
  }
}
