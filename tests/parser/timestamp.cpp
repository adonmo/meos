#include "../catch.hpp"
#include <meos/io/Parser.hpp>

TEST_CASE("timestamps are parsed", "[parser][timestamp]") {
  SECTION("only one timestamp present") {
    Parser<int> r("2012-11-01");

    REQUIRE(r.parseNextTime() == 1351728000000);
    CHECK_THROWS(r.parseNextTime());
  }

  SECTION("multiple timestamps present") {
    Parser<int> r("2012-11-01\n2012-12-02");

    REQUIRE(r.parseNextTime() == 1351728000000);
    REQUIRE(r.parseNextTime() == 1354406400000);
    CHECK_THROWS(r.parseNextTime());
  }
}
