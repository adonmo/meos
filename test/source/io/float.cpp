#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>

TEST_CASE("floats are serialized", "[serializer][float]") {
  Serializer<float> w;
  SECTION("only one float present") {
    float f = GENERATE(0.0f, 20.0f, -20.0f, 20.12f,
                       take(100, random(numeric_limits<float>::lowest(),
                                        numeric_limits<float>::max())));
    REQUIRE(stof(w.write(f)) == f);
  }
}

TEST_CASE("floats are deserialized", "[deserializer][float]") {
  SECTION("only one float present") {
    Deserializer<float> r("20.12");

    REQUIRE(r.nextValue() == 20.12f);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("multiple floats present") {
    Deserializer<float> r("20.12 11 0.1");

    REQUIRE(r.nextValue() == 20.12f);
    REQUIRE(r.nextValue() == 11.0f);
    REQUIRE(r.nextValue() == 0.1f);
    CHECK_THROWS(r.nextValue());
  }
}

TEST_CASE("float serdes", "[serializer][deserializer][float]") {
  Serializer<float> w;
  SECTION("only one float present") {
    float f = GENERATE(0.0f, 20.0f, -20.0f, 20.12f,
                       take(100, random(numeric_limits<float>::lowest(),
                                        numeric_limits<float>::max())));
    Deserializer<float> r(w.write(f));
    REQUIRE(r.nextValue() == f);
  }
}
