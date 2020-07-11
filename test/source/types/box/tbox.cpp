#include <sstream>
#include <string>

#include <catch2/catch.hpp>

#include <meos/types/box/TBox.hpp>

#include "../../common/time_utils.hpp"

TEST_CASE("TBoxes are constructed and serialized properly", "[tbox]") {
  std::stringstream output;

  SECTION("<> constructor") {
    TBox tbox;
    output << tbox;
    REQUIRE(output.str() == "TBOX()");
  }

  SECTION("<double, time_point, double, time_point> constructor") {
    TBox tbox(1, unix_time_point(2012, 1, 1), 2, unix_time_point(2012, 1, 2));
    output << tbox;
    REQUIRE(
        output.str() ==
        "TBOX((1, 2012-01-01T00:00:00+0000), (2, 2012-01-02T00:00:00+0000))");
  }

  SECTION("<double, double> constructor") {
    TBox tbox(1, 2);
    output << tbox;
    REQUIRE(output.str() == "TBOX((1,), (2,))");
  }

  SECTION("<time_point, time_point> constructor") {
    TBox tbox(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 2));
    output << tbox;
    REQUIRE(output.str() ==
            "TBOX((, 2012-01-01T00:00:00+0000), (, 2012-01-02T00:00:00+0000))");
  }

  SECTION("<double, string, double, string> constructor") {
    TBox tbox(1, "2012-01-01", 2, "2012-01-02");
    output << tbox;
    REQUIRE(
        output.str() ==
        "TBOX((1, 2012-01-01T00:00:00+0000), (2, 2012-01-02T00:00:00+0000))");
  }

  SECTION("<string, string, string, string> constructor") {
    TBox tbox("1", "2012-01-01", "2", "2012-01-02");
    output << tbox;
    REQUIRE(
        output.str() ==
        "TBOX((1, 2012-01-01T00:00:00+0000), (2, 2012-01-02T00:00:00+0000))");
  }

  SECTION("<string> constructor") {
    TBox tbox(
        "TBOX((1, 2012-01-01T00:00:00+0000), (2, 2012-01-02T00:00:00+0000))");
    output << tbox;
    REQUIRE(tbox.xmin() == 1);
    REQUIRE(tbox.tmin() == unix_time_point(2012, 1, 1));
    REQUIRE(tbox.xmax() == 2);
    REQUIRE(tbox.tmax() == unix_time_point(2012, 1, 2));
    REQUIRE(
        output.str() ==
        "TBOX((1, 2012-01-01T00:00:00+0000), (2, 2012-01-02T00:00:00+0000))");
  }
}
