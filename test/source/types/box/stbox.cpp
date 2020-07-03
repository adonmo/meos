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
    // TODO
  }
}
