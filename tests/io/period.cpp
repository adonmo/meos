#include "../catch.hpp"
#include "../common/utils.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>

TEST_CASE("Periods are serialized", "[serializer][period]") {
  Serializer<> w;
  auto left_open = GENERATE(true, false);
  auto right_open = GENERATE(true, false);
  auto period =
      make_unique<Period>(1325376000000, 1333238400000, left_open, right_open);
  string left = left_open ? "(" : "[";
  string right = right_open ? ")" : "]";
  REQUIRE(w.write(period.get()) == left + "2012-01-01T00:00:00+0000" + ", " +
                                       "2012-04-01T00:00:00+0000" + right);
}

TEST_CASE("Periods are deserialized", "[deserializer][period]") {
  SECTION("only one Period present") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    string left = left_open ? "(" : "[";
    string right = right_open ? ")" : "]";
    Deserializer<> r(left + "2012-01-01T00:00:00+0000" + ", " +
                     "2012-04-01T00:00:00+0000" + right);

    unique_ptr<Period> period = r.nextPeriod();
    REQUIRE(period->isLeftOpen() == left_open);
    REQUIRE(period->isRightOpen() == right_open);
    REQUIRE(period->lower() == 1325376000000);
    REQUIRE(period->upper() == 1333238400000);

    CHECK_THROWS(r.nextPeriod());
  }
  SECTION("multiple Periods present") {
    auto left_open_1 = GENERATE(true, false);
    auto right_open_1 = GENERATE(true, false);
    auto left_open_2 = GENERATE(true, false);
    auto right_open_2 = GENERATE(true, false);
    string period_str_1 =
        string(left_open_1 ? "(" : "[") + "2012-01-01T00:00:00+0000" + ", " +
        "2012-04-01T00:00:00+0000" + string(right_open_1 ? ")" : "]");
    string period_str_2 =
        string(left_open_2 ? "(" : "[") + "2012-01-01T00:00:00+0000" + ", " +
        "2012-04-01T00:00:00+0000" + string(right_open_2 ? ")" : "]");
    Deserializer<> r(period_str_1 + period_str_2);

    unique_ptr<Period> period_1 = r.nextPeriod();
    REQUIRE(period_1->isLeftOpen() == left_open_1);
    REQUIRE(period_1->isRightOpen() == right_open_1);
    REQUIRE(period_1->lower() == 1325376000000);
    REQUIRE(period_1->upper() == 1333238400000);

    unique_ptr<Period> period_2 = r.nextPeriod();
    REQUIRE(period_2->isLeftOpen() == left_open_2);
    REQUIRE(period_2->isRightOpen() == right_open_2);
    REQUIRE(period_2->lower() == 1325376000000);
    REQUIRE(period_2->upper() == 1333238400000);

    CHECK_THROWS(r.nextPeriod());
  }
}

TEST_CASE("Period overlap", "[period]") {
  SECTION("clear overlap") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 = *make_unique<Period>(1325376000000, 1333238400000,
                                         left_open, right_open)
                         .get();
    auto period_2 = *make_unique<Period>(1328054400000, 1335830400000,
                                         left_open, right_open)
                         .get();
    REQUIRE(period_1.overlap(period_2));
  }
  SECTION("clearly no overlap") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 = *make_unique<Period>(1325376000000, 1328054400000,
                                         left_open, right_open)
                         .get();
    auto period_2 = *make_unique<Period>(1333238400000, 1335830400000,
                                         left_open, right_open)
                         .get();
    REQUIRE(!period_1.overlap(period_2));
  }
  SECTION("borderline overlap case when only both periods are inclusive") {
    auto left_open = GENERATE(true, false);
    auto right_open = GENERATE(true, false);
    auto period_1 = *make_unique<Period>(1325376000000, 1330560000000,
                                         left_open, right_open)
                         .get();
    auto period_2 = *make_unique<Period>(1330560000000, 1335830400000,
                                         left_open, right_open)
                         .get();
    bool expected = !period_1.isRightOpen() && !period_2.isLeftOpen();
    REQUIRE(period_1.overlap(period_2) == expected);
  }
}
