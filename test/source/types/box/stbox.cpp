#include "../../common/time_utils.hpp"
#include <catch2/catch.hpp>
#include <meos/types/box/STBox.hpp>
#include <sstream>

TEST_CASE("STBoxes are constructed and serialized properly", "[stbox]") {
  std::stringstream output;

  SECTION("<> constructor") {
    STBox stbox;
    output << stbox;
    REQUIRE(output.str() == "STBOX()");
  }

  SECTION("<double, double, double, time_point, double, double, double, "
          "time_point> constructor") {
    STBox stbox(11, 12, 13, unix_time_point(2012, 1, 1), 21, 22, 23,
                unix_time_point(2012, 1, 2));
    output << stbox;
    REQUIRE(output.str() == "STBOX ZT((11, 12, 13, 2012-01-01T00:00:00+0000), "
                            "(21, 22, 23, 2012-01-02T00:00:00+0000))");
  }

  SECTION("<double, double, double, double, double, double> constructor") {
    STBox stbox(11, 12, 13, 21, 22, 23);
    output << stbox;
    REQUIRE(output.str() == "STBOX Z((11, 12, 13), (21, 22, 23))");
  }

  SECTION("<time_point, time_point> constructor") {
    STBox stbox(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 2));
    output << stbox;
    REQUIRE(output.str() == "STBOX T(( , , 2012-01-01T00:00:00+0000), ( , , "
                            "2012-01-02T00:00:00+0000))");
  }

  SECTION("<string> constructor") {
    STBox stbox("STBOX ZT((11.0, 12.0, 13.0, 2001-01-04), (21.0, 22.0, 23.0, "
                "2001-01-05))");
    output << stbox;
    REQUIRE(stbox.xmin() == 11);
    REQUIRE(stbox.ymin() == 12);
    REQUIRE(stbox.zmin() == 13);
    REQUIRE(stbox.tmin() == unix_time_point(2001, 1, 4));
    REQUIRE(stbox.xmax() == 21);
    REQUIRE(stbox.ymax() == 22);
    REQUIRE(stbox.zmax() == 23);
    REQUIRE(stbox.tmax() == unix_time_point(2001, 1, 5));
    REQUIRE(output.str() == "STBOX ZT((11, 12, 13, 2001-01-04T00:00:00+0000), "
                            "(21, 22, 23, 2001-01-05T00:00:00+0000))");
  }
}
