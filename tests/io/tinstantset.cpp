#include "../catch.hpp"
#include "../common/matchers.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstantSet.hpp>

TEMPLATE_TEST_CASE("TInstantSet are serialized", "[serializer][tinstantset]",
                   int, float) {
  Serializer<TestType> w;
  SECTION("only one value present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(100, random(numeric_limits<int>::min(),
                                       numeric_limits<int>::max())));
    auto instant = make_unique<TInstant<TestType>>(i, 1351728000000);
    set<unique_ptr<TInstant<TestType>>> instants;
    instants.insert(move(instant));
    TInstantSet<TestType> instant_set(instants);
    REQUIRE(w.write(&instant_set) ==
            "{" + w.write(i) + "@2012-11-01T00:00:00+0000}");
  }
}

TEMPLATE_TEST_CASE("TInstantSet are deserialized",
                   "[deserializer][tinstantset]", int, float) {
  SECTION("only one TInstantSet present") {
    SECTION("only one inst present") {
      Deserializer<TestType> r("{10@2012-11-01}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
      set<TInstant<TestType>> actual = unwrap(instantSet->instants);
      set<TInstant<TestType>> expected = {
          TInstant<TestType>(10, 1351728000000)};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTInstantSet());
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r(
          "{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
      set<TInstant<TestType>> actual = unwrap(instantSet->instants);
      set<TInstant<TestType>> expected = {
          TInstant<TestType>(10, 1325376000000),
          TInstant<TestType>(12, 1333238400000)};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTInstantSet());
    }
  }

  SECTION("multiple TInstantSets present") {
    Deserializer<TestType> r(
        "{10@2012-01-01 00:00:00+00} {12@2012-04-01 00:00:00+00}");

    unique_ptr<TInstantSet<TestType>> instantSet = r.nextTInstantSet();
    set<TInstant<TestType>> actual = unwrap(instantSet->instants);
    set<TInstant<TestType>> expected = {TInstant<TestType>(10, 1325376000000)};
    auto x1 = UnorderedEquals(expected);
    REQUIRE_THAT(actual, x1);

    unique_ptr<TInstantSet<TestType>> instantSet2 = r.nextTInstantSet();
    set<TInstant<TestType>> actual2 = unwrap(instantSet2->instants);
    expected = {TInstant<TestType>(12, 1333238400000)};
    auto x2 = UnorderedEquals(expected);
    REQUIRE_THAT(actual2, x2);

    CHECK_THROWS(r.nextTInstantSet());
  }
}
