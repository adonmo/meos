#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>

TEST_CASE("booleans are serialized", "[serializer][bool]") {
  Serializer<bool> w;
  SECTION("only one integer present") {
    bool i = GENERATE(true, false);
    REQUIRE(w.write(i) == (i ? "t" : "f"));
  }
}

TEST_CASE("booleans are deserialized", "[deserializer][bool]") {
  SECTION("valid values are properly deserialized") {
    string s = GENERATE("true", "false", "t", "f", "TRUE", "False", "T", "F");
    Deserializer<bool> r(s);
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    REQUIRE(r.nextValue() == (s[0] == 't'));
    CHECK_THROWS(r.nextValue());
  }

  SECTION("multiple booleans present") {
    Deserializer<bool> r("true false");

    REQUIRE(r.nextValue() == true);
    REQUIRE(r.nextValue() == false);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("invalid values") {
    string s = GENERATE("tru", "y");
    Deserializer<bool> r(s);
    CHECK_THROWS(r.nextValue());
  }
}

TEST_CASE("boolean serdes", "[serializer][deserializer][bool]") {
  Serializer<bool> w;
  SECTION("only one boolean present") {
    bool i = GENERATE(true, false);
    Deserializer<bool> r(w.write(i));
    REQUIRE(r.nextValue() == i);
  }
}
