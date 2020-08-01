#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/time/Period.hpp>
#include <string>

#include "../common/time_utils.hpp"
#include "../common/utils.hpp"

using namespace meos;
using namespace std;

TEST_CASE("Periods are serialized", "[serializer][period]") {
  Serializer<> w;
  auto lower_inc = GENERATE(true, false);
  auto upper_inc = GENERATE(true, false);
  auto period = make_unique<Period>(unix_time_point(2012, 1, 1), unix_time_point(2012, 4, 1),
                                    lower_inc, upper_inc);
  string left = lower_inc ? "[" : "(";
  string right = upper_inc ? "]" : ")";
  REQUIRE(w.write(period.get())
          == left + "2012-01-01T00:00:00+0000" + ", " + "2012-04-01T00:00:00+0000" + right);
}

TEST_CASE("Periods are deserialized", "[deserializer][period]") {
  SECTION("only one Period present") {
    auto lower_inc = GENERATE(true, false);
    auto upper_inc = GENERATE(true, false);
    string left = lower_inc ? "[" : "(";
    string right = upper_inc ? "]" : ")";
    Deserializer<> r(left + "2012-01-01T00:00:00+0000" + ", " + "2012-04-01T00:00:00+0000" + right);

    unique_ptr<Period> period = r.nextPeriod();
    REQUIRE(period->lower_inc() == lower_inc);
    REQUIRE(period->upper_inc() == upper_inc);
    REQUIRE(period->lower() == unix_time_point(2012, 1, 1));
    REQUIRE(period->upper() == unix_time_point(2012, 4, 1));

    CHECK_THROWS(r.nextPeriod());
  }
  SECTION("multiple Periods present") {
    auto lower_inc_1 = GENERATE(true, false);
    auto upper_inc_1 = GENERATE(true, false);
    auto lower_inc_2 = GENERATE(true, false);
    auto upper_inc_2 = GENERATE(true, false);
    string period_str_1 = string(lower_inc_1 ? "[" : "(") + "2012-01-01T00:00:00+0000" + ", "
                          + "2012-04-01T00:00:00+0000" + string(upper_inc_1 ? "]" : ")");
    string period_str_2 = string(lower_inc_2 ? "[" : "(") + "2012-01-01T00:00:00+0000" + ", "
                          + "2012-04-01T00:00:00+0000" + string(upper_inc_2 ? "]" : ")");
    Deserializer<> r(period_str_1 + period_str_2);

    unique_ptr<Period> period_1 = r.nextPeriod();
    REQUIRE(period_1->lower_inc() == lower_inc_1);
    REQUIRE(period_1->upper_inc() == upper_inc_1);
    REQUIRE(period_1->lower() == unix_time_point(2012, 1, 1));
    REQUIRE(period_1->upper() == unix_time_point(2012, 4, 1));

    unique_ptr<Period> period_2 = r.nextPeriod();
    REQUIRE(period_2->lower_inc() == lower_inc_2);
    REQUIRE(period_2->upper_inc() == upper_inc_2);
    REQUIRE(period_2->lower() == unix_time_point(2012, 1, 1));
    REQUIRE(period_2->upper() == unix_time_point(2012, 4, 1));

    CHECK_THROWS(r.nextPeriod());
  }
}
