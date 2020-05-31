#include "../catch.hpp"
#include "../common/time_utils.hpp"
#include "../common/utils.hpp"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>

TEST_CASE("Periods are serialized", "[serializer][period]") {
  Serializer<> w;
  auto left_open = GENERATE(true, false);
  auto right_open = GENERATE(true, false);
  auto period = make_unique<Period>(
      unix_time(2012, 1, 1), unix_time(2012, 4, 1), left_open, right_open);
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
    REQUIRE(period->lower() == unix_time(2012, 1, 1));
    REQUIRE(period->upper() == unix_time(2012, 4, 1));

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
    REQUIRE(period_1->lower() == unix_time(2012, 1, 1));
    REQUIRE(period_1->upper() == unix_time(2012, 4, 1));

    unique_ptr<Period> period_2 = r.nextPeriod();
    REQUIRE(period_2->isLeftOpen() == left_open_2);
    REQUIRE(period_2->isRightOpen() == right_open_2);
    REQUIRE(period_2->lower() == unix_time(2012, 1, 1));
    REQUIRE(period_2->upper() == unix_time(2012, 4, 1));

    CHECK_THROWS(r.nextPeriod());
  }
}
