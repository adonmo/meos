#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>

using namespace meos;
using namespace std;

TEST_CASE("integers are serialized", "[serializer][int]") {
  Serializer<int> w;
  SECTION("only one integer present") {
    int i = GENERATE(0, 1, -1, 2012, 756772544,
                     take(100, random(numeric_limits<int>::min(), numeric_limits<int>::max())));
    REQUIRE(stoi(w.write(i)) == i);
  }
}

TEST_CASE("integers are deserialized", "[deserializer][int]") {
  SECTION("only one integer present") {
    Deserializer<int> r("2012");

    REQUIRE(r.nextValue() == 2012);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("multiple integers present") {
    Deserializer<int> r("2012 11 01");

    REQUIRE(r.nextValue() == 2012);
    REQUIRE(r.nextValue() == 11);
    REQUIRE(r.nextValue() == 01);
    CHECK_THROWS(r.nextValue());
  }
}

TEST_CASE("integer serdes", "[serializer][deserializer][int]") {
  Serializer<int> w;
  SECTION("only one integer present") {
    int i = GENERATE(0, 1, -1, 2012,
                     take(100, random(numeric_limits<int>::min(), numeric_limits<int>::max())));
    Deserializer<int> r(w.write(i));
    REQUIRE(r.nextValue() == i);
  }
}
