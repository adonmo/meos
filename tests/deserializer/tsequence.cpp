#include "../catch.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/types/temporal/TSequence.hpp>

TEMPLATE_TEST_CASE("TSequence are deserialized", "[deserializer][temporal]",
                   int, float) {
  SECTION("only one TSequence present") {
    SECTION("only one inst present") {
      Deserializer<TestType> r("[10@2012-11-01)");

      unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
      vector<TInstant<TestType>> actual = unwrap(tseq->instants);
      vector<TInstant<TestType>> v = {TInstant<TestType>(10, 1351728000000)};
      auto x = Catch::Matchers::Equals(v);
      REQUIRE_THAT(actual, x);
      REQUIRE(tseq->left_open == false);
      REQUIRE(tseq->right_open == true);

      CHECK_THROWS(r.nextTSequence());
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r(
          "(10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00)");

      unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
      vector<TInstant<TestType>> actual = unwrap(tseq->instants);
      vector<TInstant<TestType>> v = {TInstant<TestType>(10, 1325376000000),
                                      TInstant<TestType>(12, 1333238400000)};
      auto x = Catch::Matchers::Equals(v);
      REQUIRE_THAT(actual, x);
      REQUIRE(tseq->left_open == true);
      REQUIRE(tseq->right_open == true);

      CHECK_THROWS(r.nextTSequence());
    }
  }

  SECTION("multiple TSequences present") {
    Deserializer<TestType> r(
        "(10@2012-01-01 00:00:00+00] [12@2012-04-01 00:00:00+00]");

    unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
    vector<TInstant<TestType>> actual = unwrap(tseq->instants);
    vector<TInstant<TestType>> v = {TInstant<TestType>(10, 1325376000000)};
    auto x1 = Catch::Matchers::Equals(v);
    REQUIRE_THAT(actual, x1);
    REQUIRE(tseq->left_open == true);
    REQUIRE(tseq->right_open == false);

    unique_ptr<TSequence<TestType>> tseq2 = r.nextTSequence();
    vector<TInstant<TestType>> actual2 = unwrap(tseq2->instants);
    v = {TInstant<TestType>(12, 1333238400000)};
    auto x2 = Catch::Matchers::Equals(v);
    REQUIRE_THAT(actual2, x2);
    REQUIRE(tseq2->left_open == false);
    REQUIRE(tseq2->right_open == false);

    CHECK_THROWS(r.nextTSequence());
  }
}
