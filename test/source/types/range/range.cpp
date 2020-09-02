#include <catch2/catch.hpp>
#include <meos/types/range/Range.hpp>

#include "../../common/utils.hpp"

using namespace meos;

TEST_CASE("ranges are validated and constructed properly", "[range]") {
  SECTION("reads from istream") {
    RangeInt range;
    stringstream ss("  [  10  ,      20 )");
    ss >> range;
    REQUIRE(range.lower() == 10);
    REQUIRE(range.upper() == 20);
    REQUIRE(range.lower_inc() == true);
    REQUIRE(range.upper_inc() == false);
  }

  SECTION("all constructors work") {
    unique_ptr<RangeInt> range;
    SECTION("constructors with bounds specified") {
      SECTION("normal constructor") { range = make_unique<RangeInt>(10, 20, false, true); }
      SECTION("string constructor") { range = make_unique<RangeInt>("(10, 20]"); }
      REQUIRE(range->lower_inc() == false);
      REQUIRE(range->upper_inc() == true);
    }
    SECTION("constructors with default bounds") {
      SECTION("normal constructor") { range = make_unique<RangeInt>(10, 20); }
      SECTION("string constructor") { range = make_unique<RangeInt>("[10, 20)"); }
      REQUIRE(range->lower_inc() == true);
      REQUIRE(range->upper_inc() == false);
    }
    REQUIRE(range->lower() == 10);
    REQUIRE(range->upper() == 20);
  }

  SECTION("clearly valid") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto lower = 10;
    auto upper = 20;
    RangeInt range(lower, upper, lower_inc, upper_inc);
    REQUIRE(range.lower() == lower);
    REQUIRE(range.upper() == upper);
    REQUIRE(range.lower_inc() == lower_inc);
    REQUIRE(range.upper_inc() == upper_inc);
  }

  SECTION("clearly invalid") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto lower = 20;
    auto upper = 10;
    REQUIRE_THROWS_AS((RangeInt{lower, upper, lower_inc, upper_inc}), std::invalid_argument);
  }

  SECTION("edge case where lower == upper") {
    int v;
    bool lower_inc, upper_inc, should_be_valid;
    std::tie(v, lower_inc, upper_inc, should_be_valid) = GENERATE(table<int, bool, bool, bool>({
        {10, true, true, true},
        {10, true, false, false},
        {10, false, false, false},
        {10, false, true, false},
    }));
    if (!should_be_valid) {
      REQUIRE_THROWS_AS((RangeInt{v, v, lower_inc, upper_inc}), std::invalid_argument);
    } else {
      REQUIRE_NOTHROW(RangeInt{v, v, lower_inc, upper_inc});
    }
  }
}

TEMPLATE_TEST_CASE("Range shift", "[range]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  TestType lower = 0;
  TestType upper = 1000;
  auto range_in = Range<TestType>(upper, lower, lower_inc, upper_inc);
  TestType const offset = 100L;
  auto range_out = *range_in.shift(offset).get();
  REQUIRE(range_out.lower() == upper + offset);
  REQUIRE(range_out.upper() == lower + offset);
  REQUIRE(range_out.lower_inc() == lower_inc);
  REQUIRE(range_out.upper_inc() == upper_inc);
}

TEMPLATE_TEST_CASE("Range overlap", "[range]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  SECTION("clear overlap") {
    auto range_1 = Range<TestType>(1, 4, lower_inc, upper_inc);
    auto range_2 = Range<TestType>(2, 5, lower_inc, upper_inc);
    REQUIRE(range_1.overlap(range_2));
  }
  SECTION("clearly no overlap") {
    auto range_1 = Range<TestType>(1, 2, lower_inc, upper_inc);
    auto range_2 = Range<TestType>(4, 5, lower_inc, upper_inc);
    REQUIRE(!range_1.overlap(range_2));
  }
  SECTION("borderline overlap case when only both ranges are inclusive") {
    auto range_1 = Range<TestType>(1, 3, lower_inc, upper_inc);
    auto range_2 = Range<TestType>(3, 5, lower_inc, upper_inc);
    bool expected = range_1.upper_inc() && range_2.lower_inc();
    REQUIRE(range_1.overlap(range_2) == expected);
  }
}

TEMPLATE_TEST_CASE("Range contains value", "[range]", int, float) {
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  SECTION("clearly contains value") {
    auto range = Range<TestType>(1, 4, lower_inc, upper_inc);
    REQUIRE(range.contains(2));
    REQUIRE(range.contains(3));
  }
  SECTION("clearly does not contains value") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto range = Range<TestType>(1, 4, lower_inc, upper_inc);
    REQUIRE(!range.contains(0));
    REQUIRE(!range.contains(5));
  }
  SECTION("borderline contains value only on inclusive end") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    auto range = Range<TestType>(1, 4, lower_inc, upper_inc);
    REQUIRE(range.lower_inc() == range.contains(1));
    REQUIRE(range.upper_inc() == range.contains(4));
  }
}
