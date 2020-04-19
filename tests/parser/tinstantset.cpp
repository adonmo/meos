#include "../catch.hpp"
#include "../common/matchers.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Parser.hpp>
#include <meos/types/temporal/TInstantSet.hpp>

TEMPLATE_TEST_CASE("TInstantSet are parsed", "[parser][tinstantset]", int,
                   float) {
  SECTION("only one TInstantSet present") {
    SECTION("only one inst present") {
      Parser<TestType> r("{10@2012-11-01}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.parseNextTInstantSet();
      set<TInstant<TestType>> actual = unwrap(instantSet->instants);
      set<TInstant<TestType>> expected = {
          TInstant<TestType>(10, 1351728000000)};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.parseNextTInstantSet());
    }
    SECTION("multiple inst present") {
      Parser<TestType> r(
          "{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");

      unique_ptr<TInstantSet<TestType>> instantSet = r.parseNextTInstantSet();
      set<TInstant<TestType>> actual = unwrap(instantSet->instants);
      set<TInstant<TestType>> expected = {
          TInstant<TestType>(10, 1325376000000),
          TInstant<TestType>(12, 1333238400000)};
      auto x = UnorderedEquals(expected);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.parseNextTInstantSet());
    }
  }

  SECTION("multiple TInstantSets present") {
    Parser<TestType> r(
        "{10@2012-01-01 00:00:00+00} {12@2012-04-01 00:00:00+00}");

    unique_ptr<TInstantSet<TestType>> instantSet = r.parseNextTInstantSet();
    set<TInstant<TestType>> actual = unwrap(instantSet->instants);
    set<TInstant<TestType>> expected = {TInstant<TestType>(10, 1325376000000)};
    auto x1 = UnorderedEquals(expected);
    REQUIRE_THAT(actual, x1);

    unique_ptr<TInstantSet<TestType>> instantSet2 = r.parseNextTInstantSet();
    set<TInstant<TestType>> actual2 = unwrap(instantSet2->instants);
    expected = {TInstant<TestType>(12, 1333238400000)};
    auto x2 = UnorderedEquals(expected);
    REQUIRE_THAT(actual2, x2);

    CHECK_THROWS(r.parseNextTInstantSet());
  }
}
