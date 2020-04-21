#include "../catch.hpp"
#include "../common/matchers.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>

TEMPLATE_TEST_CASE("Temporal are deserialized", "[deserializer][temporal]", int,
                   float) {
  SECTION("only one Temporal present") {
    SECTION("only one inst present") {
      SECTION("in a instant duration") {
        Deserializer<TestType> r("10@2012-11-01");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (static_cast<TInstant<TestType> *>(temporal.get()));
        REQUIRE(casted->getValue() == 10);
        REQUIRE(casted->getT() == 1351728000000);

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("in a instantset duration") {
        Deserializer<TestType> r("{10@2012-11-01}");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (static_cast<TInstantSet<TestType> *>(temporal.get()));
        set<TInstant<TestType>> actual = unwrap(casted->instants);
        set<TInstant<TestType>> s = {TInstant<TestType>(10, 1351728000000)};
        auto x = UnorderedEquals(s);
        REQUIRE_THAT(actual, x);

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("in a sequence duration") {
        Deserializer<TestType> r("[10@2012-11-01)");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (static_cast<TSequence<TestType> *>(temporal.get()));
        vector<TInstant<TestType>> actual = unwrap(casted->instants);
        vector<TInstant<TestType>> v = {TInstant<TestType>(10, 1351728000000)};
        auto x = Catch::Matchers::Equals(v);
        REQUIRE_THAT(actual, x);
        REQUIRE(casted->left_open == false);
        REQUIRE(casted->right_open == true);

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("shouldn't be able to case a instantset duration to a sequence "
              "duration") {
        Deserializer<TestType> r("{10@2012-11-01}");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (static_cast<TSequence<TestType> *>(temporal.get()));
        CHECK_THROWS(unwrap(casted->instants));
      }
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r(
          "{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");

      unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
      auto casted = (static_cast<TInstantSet<TestType> *>(temporal.get()));
      set<TInstant<TestType>> actual = unwrap(casted->instants);
      set<TInstant<TestType>> v = {TInstant<TestType>(10, 1325376000000),
                                   TInstant<TestType>(12, 1333238400000)};
      auto x = UnorderedEquals(v);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTemporal());
    }
  }

  SECTION("multiple Temporals present") {
    Deserializer<TestType> r(
        "{10@2012-01-01 00:00:00+00} {12@2012-04-01 00:00:00+00}");

    unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
    auto casted = (static_cast<TInstantSet<TestType> *>(temporal.get()));
    set<TInstant<TestType>> actual = unwrap(casted->instants);
    set<TInstant<TestType>> v = {TInstant<TestType>(10, 1325376000000)};
    auto x1 = UnorderedEquals(v);
    REQUIRE_THAT(actual, x1);

    unique_ptr<Temporal<TestType>> temporal2 = r.nextTemporal();
    auto casted2 = (static_cast<TInstantSet<TestType> *>(temporal2.get()));
    set<TInstant<TestType>> actual2 = unwrap(casted2->instants);
    v = {TInstant<TestType>(12, 1333238400000)};
    auto x2 = UnorderedEquals(v);
    REQUIRE_THAT(actual2, x2);

    CHECK_THROWS(r.nextTemporal());
  }
}

// r.read("10@2012-01-01 00:00:00+00");
// r.read("[10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00]");
// r.read("[10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00)");
// DONE r.read("{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");
// r.read("{[10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00],
// (10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00)}");

// DONE r.read("10.1@2012-01-01 00:00:00+00");
