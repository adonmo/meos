#include "../catch.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TSequence.hpp>

TEMPLATE_TEST_CASE("TSequence are serialized", "[serializer][tsequence]", int,
                   float) {
  Serializer<TestType> w;
  SECTION("only one value present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(10, random(numeric_limits<int>::min(),
                                      numeric_limits<int>::max())));
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto instant = make_unique<TInstant<TestType>>(i, 1351728000000);
    vector<unique_ptr<TInstant<TestType>>> instants;
    instants.push_back(move(instant));
    TSequence<TestType> sequence(instants, left_open, right_open);
    char left = left_open ? '(' : '[';
    char right = right_open ? ')' : ']';
    REQUIRE(w.write(&sequence) ==
            left + w.write(i) + "@2012-11-01T00:00:00+0000" + right);
  }
}

TEMPLATE_TEST_CASE("TSequence are deserialized", "[deserializer][tsequence]",
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
