#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <string>

using namespace meos;
using namespace std;

TEST_CASE("text values are serialized", "[serializer][text]") {
  Serializer<string> w;
  SECTION("only one integer present") {
    auto s = GENERATE(as<std::string>{}, "a", "bb", "ccc");
    REQUIRE(w.write(s) == '"' + s + '"');
  }
}

TEST_CASE("text values are deserialized", "[deserializer][text]") {
  SECTION("valid unqoted values are properly deserialized") {
    auto s = GENERATE(as<std::string>{}, "a", "bb", "ccc");
    Deserializer<string> r(s);
    REQUIRE(r.nextValue() == s);
    CHECK_THROWS(r.nextValue());
  }

  SECTION("valid quoted values are properly deserialized") {
    auto s = GENERATE(as<std::string>{}, "a", "bb", "cc c", "");
    Deserializer<string> r('"' + s + '"');
    REQUIRE(r.nextValue() == s);
    CHECK_THROWS(r.nextValue());
  }
}

TEST_CASE("text serdes", "[serializer][deserializer][text]") {
  Serializer<string> w;
  SECTION("only one text value present") {
    auto s = GENERATE(as<std::string>{}, "a", "bb", "cc c");
    Deserializer<string> r(w.write(s));
    REQUIRE(r.nextValue() == s);
  }
}
