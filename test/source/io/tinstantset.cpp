#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <string>

#include "../common/matchers.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"

using namespace std;

TEMPLATE_TEST_CASE("TInstantSet are serialized", "[serializer][tinstantset]", int, float) {
  Serializer<TestType> w;
  SECTION("only one value present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(30, random(numeric_limits<int>::min(), numeric_limits<int>::max())));
    auto instant = TInstant<TestType>(i, unix_time_point(2012, 11, 1));
    set<TInstant<TestType>> instants;
    instants.insert(instant);
    TInstantSet<TestType> instant_set(instants);
    REQUIRE(w.write(&instant_set) == "{" + w.write(i) + "@2012-11-01T00:00:00+0000}");
  }
  SECTION("multiple values present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(6, random(numeric_limits<int>::min(), numeric_limits<int>::max())));
    auto j = GENERATE(0, 1, -1, 2012, 756772544,
                      take(6, random(numeric_limits<int>::min(), numeric_limits<int>::max())));
    auto instant1 = TInstant<TestType>(i, unix_time_point(2012, 11, 1));
    auto instant2 = TInstant<TestType>(j, unix_time_point(2012, 11, 1));
    set<TInstant<TestType>> instants;
    instants.insert(instant1);
    instants.insert(instant2);
    TInstantSet<TestType> instant_set(instants);
    string serialized = w.write(&instant_set);
    REQUIRE(serialized.size() > 2);
    REQUIRE(serialized[0] == '{');
    REQUIRE(serialized[serialized.size() - 1] == '}');
    set<string> actual = split_into_set(serialized.substr(1, serialized.size() - 2), ", ");
    set<string> expected = {
        w.write(i) + "@2012-11-01T00:00:00+0000",
        w.write(j) + "@2012-11-01T00:00:00+0000",
    };
    REQUIRE_THAT(actual, UnorderedEquals(expected));
  }
}

TEMPLATE_TEST_CASE("TInstantSet are deserialized", "[deserializer][tinstantset]", int, float) {
  SECTION("only one TInstantSet present") {
    SECTION("only one inst present") {
      Deserializer<TestType> r("{10@2012-11-01}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
      set<TInstant<TestType>> actual = instantSet->instants();
      set<TInstant<TestType>> expected = {TInstant<TestType>(10, unix_time_point(2012, 11, 1))};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTInstantSet());
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r("{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
      set<TInstant<TestType>> actual = instantSet->instants();
      set<TInstant<TestType>> expected = {TInstant<TestType>(10, unix_time_point(2012, 1, 1)),
                                          TInstant<TestType>(12, unix_time_point(2012, 4, 1))};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTInstantSet());
    }
  }

  SECTION("multiple TInstantSets present") {
    Deserializer<TestType> r("{10@2012-01-01 00:00:00+00} {12@2012-04-01 00:00:00+00}");

    unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
    set<TInstant<TestType>> actual = instantSet->instants();
    set<TInstant<TestType>> expected = {TInstant<TestType>(10, unix_time_point(2012, 1, 1))};
    auto x1 = UnorderedEquals(expected);
    REQUIRE_THAT(actual, x1);

    unique_ptr<TInstantSet<TestType>> instantSet2 = r.nextTInstantSet();
    set<TInstant<TestType>> actual2 = instantSet2->instants();
    expected = {TInstant<TestType>(12, unix_time_point(2012, 4, 1))};
    auto x2 = UnorderedEquals(expected);
    REQUIRE_THAT(actual2, x2);

    CHECK_THROWS(r.nextTInstantSet());
  }
}
