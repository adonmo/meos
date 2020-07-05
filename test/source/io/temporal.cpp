#include "../common/matchers.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"
#include <catch2/catch.hpp>
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
        auto casted = (dynamic_cast<TInstant<TestType> *>(temporal.get()));
        REQUIRE(casted->getValue() == 10);
        REQUIRE(casted->getTimestamp() == unix_time_point(2012, 11, 1));

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("in a instantset duration") {
        Deserializer<TestType> r("{10@2012-11-01}");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (dynamic_cast<TInstantSet<TestType> *>(temporal.get()));
        set<TInstant<TestType>> actual = unwrap(casted->m_instants);
        set<TInstant<TestType>> s = {
            TInstant<TestType>(10, unix_time_point(2012, 11, 1))};
        auto x = UnorderedEquals(s);
        REQUIRE_THAT(actual, x);

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("in a sequence duration") {
        Deserializer<TestType> r("[10@2012-11-01)");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (dynamic_cast<TSequence<TestType> *>(temporal.get()));
        set<TInstant<TestType>> actual = casted->m_instants;
        set<TInstant<TestType>> s = {
            TInstant<TestType>(10, unix_time_point(2012, 11, 1))};
        auto x = UnorderedEquals(s);
        REQUIRE_THAT(actual, x);
        REQUIRE(casted->lower_inc() == true);
        REQUIRE(casted->upper_inc() == false);

        CHECK_THROWS(r.nextTemporal());
      }
      SECTION("shouldn't be able to cast a instantset duration to a sequence "
              "duration") {
        Deserializer<TestType> r("{10@2012-11-01}");

        unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
        auto casted = (dynamic_cast<TSequence<TestType> *>(temporal.get()));
        REQUIRE(casted == NULL);
      }
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r(
          "{10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00}");

      unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
      auto casted = (dynamic_cast<TInstantSet<TestType> *>(temporal.get()));
      set<TInstant<TestType>> actual = unwrap(casted->m_instants);
      set<TInstant<TestType>> v = {
          TInstant<TestType>(10, unix_time_point(2012, 1, 1)),
          TInstant<TestType>(12, unix_time_point(2012, 4, 1))};
      auto x = UnorderedEquals(v);
      REQUIRE_THAT(actual, x);

      CHECK_THROWS(r.nextTemporal());
    }
  }

  SECTION("multiple Temporals present") {
    Deserializer<TestType> r(
        "{10@2012-01-01 00:00:00+00} {12@2012-04-01 00:00:00+00}");

    unique_ptr<Temporal<TestType>> temporal = r.nextTemporal();
    auto casted = (dynamic_cast<TInstantSet<TestType> *>(temporal.get()));
    set<TInstant<TestType>> actual = unwrap(casted->m_instants);
    set<TInstant<TestType>> v = {
        TInstant<TestType>(10, unix_time_point(2012, 1, 1))};
    auto x1 = UnorderedEquals(v);
    REQUIRE_THAT(actual, x1);

    unique_ptr<Temporal<TestType>> temporal2 = r.nextTemporal();
    auto casted2 = (dynamic_cast<TInstantSet<TestType> *>(temporal2.get()));
    set<TInstant<TestType>> actual2 = unwrap(casted2->m_instants);
    v = {TInstant<TestType>(12, unix_time_point(2012, 4, 1))};
    auto x2 = UnorderedEquals(v);
    REQUIRE_THAT(actual2, x2);

    CHECK_THROWS(r.nextTemporal());
  }
}
