#include "../catch.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"
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
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto instant = make_unique<TInstant<TestType>>(i, unix_time(2012, 11, 1));
    vector<unique_ptr<TInstant<TestType>>> instants;
    instants.push_back(move(instant));
    TSequence<TestType> sequence(instants, lower_inc, upper_inc);
    char left = lower_inc ? '[' : '(';
    char right = upper_inc ? ']' : ')';
    REQUIRE(w.write(&sequence) ==
            left + w.write(i) + "@2012-11-01T00:00:00+0000" + right);
  }
  SECTION("multiple values present") {
    auto i = GENERATE(0, 1, -1, 2012, 756772544,
                      take(3, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
    auto j = GENERATE(0, 1, -1, 2012, 756772544,
                      take(3, random(numeric_limits<int>::min(),
                                     numeric_limits<int>::max())));
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto instant1 = make_unique<TInstant<TestType>>(i, unix_time(2012, 11, 1));
    auto instant2 = make_unique<TInstant<TestType>>(j, unix_time(2012, 11, 1));
    vector<unique_ptr<TInstant<TestType>>> instants;
    instants.push_back(move(instant1));
    instants.push_back(move(instant2));
    TSequence<TestType> sequence(instants, lower_inc, upper_inc);
    char left = lower_inc ? '[' : '(';
    char right = upper_inc ? ']' : ')';
    string serialized = w.write(&sequence);
    REQUIRE(serialized.size() > 2);
    REQUIRE(serialized[0] == left);
    REQUIRE(serialized[serialized.size() - 1] == right);
    vector<string> actual =
        split(serialized.substr(1, serialized.size() - 2), ", ");
    vector<string> expected = {
        w.write(i) + "@2012-11-01T00:00:00+0000",
        w.write(j) + "@2012-11-01T00:00:00+0000",
    };
    REQUIRE_THAT(actual, Catch::Matchers::Equals(expected));
  }
}

TEMPLATE_TEST_CASE("TSequence are deserialized", "[deserializer][tsequence]",
                   int, float) {
  SECTION("only one TSequence present") {
    SECTION("only one inst present") {
      Deserializer<TestType> r("[10@2012-11-01)");

      unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
      vector<TInstant<TestType>> actual = unwrap(tseq->instants);
      vector<TInstant<TestType>> v = {
          TInstant<TestType>(10, unix_time(2012, 11, 1))};
      auto x = Catch::Matchers::Equals(v);
      REQUIRE_THAT(actual, x);
      REQUIRE(tseq->lower_inc == true);
      REQUIRE(tseq->upper_inc == false);

      CHECK_THROWS(r.nextTSequence());
    }
    SECTION("multiple inst present") {
      Deserializer<TestType> r(
          "(10@2012-01-01 00:00:00+00, 12@2012-04-01 00:00:00+00)");

      unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
      vector<TInstant<TestType>> actual = unwrap(tseq->instants);
      vector<TInstant<TestType>> v = {
          TInstant<TestType>(10, unix_time(2012, 1, 1)),
          TInstant<TestType>(12, unix_time(2012, 4, 1))};
      auto x = Catch::Matchers::Equals(v);
      REQUIRE_THAT(actual, x);
      REQUIRE(tseq->lower_inc == false);
      REQUIRE(tseq->upper_inc == false);

      CHECK_THROWS(r.nextTSequence());
    }
  }

  SECTION("multiple TSequences present") {
    Deserializer<TestType> r(
        "(10@2012-01-01 00:00:00+00] [12@2012-04-01 00:00:00+00]");

    unique_ptr<TSequence<TestType>> tseq = r.nextTSequence();
    vector<TInstant<TestType>> actual = unwrap(tseq->instants);
    vector<TInstant<TestType>> v1 = {
        TInstant<TestType>(10, unix_time(2012, 1, 1))};
    auto x1 = Catch::Matchers::Equals(v1);
    REQUIRE_THAT(actual, x1);
    REQUIRE(tseq->lower_inc == false);
    REQUIRE(tseq->upper_inc == true);

    unique_ptr<TSequence<TestType>> tseq2 = r.nextTSequence();
    vector<TInstant<TestType>> actual2 = unwrap(tseq2->instants);
    vector<TInstant<TestType>> v2 = {
        TInstant<TestType>(12, unix_time(2012, 4, 1))};
    auto x2 = Catch::Matchers::Equals(v2);
    REQUIRE_THAT(actual2, x2);
    REQUIRE(tseq2->lower_inc == true);
    REQUIRE(tseq2->upper_inc == true);

    CHECK_THROWS(r.nextTSequence());
  }
}
