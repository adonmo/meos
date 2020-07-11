#include <sstream>
#include <string>

#include <catch2/catch.hpp>

#include <meos/types/box/STBox.hpp>

#include "../../common/time_utils.hpp"

TEST_CASE("STBoxes are constructed and serialized properly", "[stbox]") {
  std::stringstream output;
  STBox stbox;
  string expected;

  SECTION("Default empty constructor") { expected = "STBOX()"; }

  SECTION("STBOX ZT") {
    SECTION("time as time_point") {
      stbox = STBox(11, 12, 13, unix_time_point(2012, 1, 1), 21, 22, 23,
                    unix_time_point(2012, 1, 2));
    }

    SECTION("time as string") {
      stbox = STBox(11, 12, 13, "2012-01-01", 21, 22, 23, "2012-01-02");
    }
    expected = "STBOX ZT((11, 12, 13, 2012-01-01T00:00:00+0000), "
               "(21, 22, 23, 2012-01-02T00:00:00+0000))";
  }

  SECTION("STBOX Z") {
    stbox = STBox(11, 12, 13, 21, 22, 23);
    expected = "STBOX Z((11, 12, 13), (21, 22, 23))";
  }

  SECTION("STBOX T") {
    SECTION("with xy") {
      SECTION("time as time_point") {
        stbox = STBox(11, 12, unix_time_point(2012, 1, 1), 21, 22,
                      unix_time_point(2012, 1, 2));
      }

      SECTION("time as string") {
        stbox = STBox(11, 12, "2012-01-01", 21, 22, "2012-01-02");
      }
      expected = "STBOX T((11, 12, 2012-01-01T00:00:00+0000), "
                 "(21, 22, 2012-01-02T00:00:00+0000))";
    }
    SECTION("without xy") {
      SECTION("time as time_point") {
        stbox = STBox(unix_time_point(2012, 1, 1), unix_time_point(2012, 1, 2),
                      SRID_DEFAULT);
      }

      SECTION("time as string") { stbox = STBox("2012-01-01", "2012-01-02"); }

      expected = "STBOX T(( , , 2012-01-01T00:00:00+0000), ( , , "
                 "2012-01-02T00:00:00+0000))";
    }
  }

  SECTION("STBOX") {
    stbox = STBox(11, 12, 21, 22);
    expected = "STBOX((11, 12), (21, 22))";
  }

  SECTION("geodetic constructors") {
    SECTION("GEODSTBOX T") {
      SECTION("with xyz") {
        SECTION("time as time_point") {
          stbox = STBox(11, 12, 13, unix_time_point(2012, 1, 1), 21, 22, 23,
                        unix_time_point(2012, 1, 2), SRID_DEFAULT, true);
        }
        SECTION("time as string") {
          stbox = STBox(11, 12, 13, "2012-01-01", 21, 22, 23, "2012-01-02",
                        SRID_DEFAULT, true);
        }
        expected =
            "SRID=4326;GEODSTBOX T((11, 12, 13, 2012-01-01T00:00:00+0000), "
            "(21, 22, 23, 2012-01-02T00:00:00+0000))";
      }
      SECTION("without xyz") {
        SECTION("GEODSTBOX T") {
          SECTION("time as time_point") {
            stbox = STBox(unix_time_point(2012, 1, 1),
                          unix_time_point(2012, 1, 2), SRID_DEFAULT, true);
          }
          SECTION("time as string") {
            stbox = STBox("2012-01-01", "2012-01-02", SRID_DEFAULT, true);
          }
          expected = "GEODSTBOX T(( , , 2012-01-01T00:00:00+0000), "
                     "( , , 2012-01-02T00:00:00+0000))";
        }
      }
    }

    SECTION("GEODSTBOX") {
      stbox = STBox(11, 12, 13, 21, 22, 23, SRID_DEFAULT, true);
      expected = "SRID=4326;GEODSTBOX((11, 12, 13), (21, 22, 23))";
    }
  }

  SECTION("constructors with SRID specified") {
    SECTION("STBOX T") {
      stbox = STBox(11, 12, unix_time_point(2012, 1, 1), 21, 22,
                    unix_time_point(2012, 1, 2), 5676);
      expected = "SRID=5676;STBOX T((11, 12, 2012-01-01T00:00:00+0000), "
                 "(21, 22, 2012-01-02T00:00:00+0000))";
    }
    SECTION("GEODSTBOX") {
      stbox = STBox(11, 12, 13, 21, 22, 23, 4326, true);
      expected = "SRID=4326;GEODSTBOX((11, 12, 13), (21, 22, 23))";
    }
  }

  SECTION("string constructor") {
    stbox = STBox("SRID=4326;STBOX ZT((11.0, 12.0, 13.0, 2001-01-04), "
                  "(21.0, 22.0, 23.0, 2001-01-05))");
    REQUIRE(stbox.xmin() == 11);
    REQUIRE(stbox.ymin() == 12);
    REQUIRE(stbox.zmin() == 13);
    REQUIRE(stbox.tmin() == unix_time_point(2001, 1, 4));
    REQUIRE(stbox.xmax() == 21);
    REQUIRE(stbox.ymax() == 22);
    REQUIRE(stbox.zmax() == 23);
    REQUIRE(stbox.tmax() == unix_time_point(2001, 1, 5));
    REQUIRE(stbox.srid() == 4326);
    REQUIRE(stbox.geodetic() == false);
    expected = "SRID=4326;STBOX ZT((11, 12, 13, 2001-01-04T00:00:00+0000), "
               "(21, 22, 23, 2001-01-05T00:00:00+0000))";
  }

  output << stbox;
  REQUIRE(output.str() == expected);
}

TEST_CASE("STBoxes throw when constructed with bad arguments", "[stbox]") {
  SECTION("throw when SRID specified, but coordinates not present") {
    SECTION("string constructor") {
      REQUIRE_THROWS_AS(
          (STBox{"SRID=4326;GEODSTBOX T(( , , 2012-01-01T00:00:00+0000), "
                 "( , , 2012-01-02T00:00:00+0000))"}),
          std::invalid_argument);
    }
    SECTION("two strings constructor") {
      REQUIRE_THROWS_AS((STBox{"2012-01-01", "2012-01-02", 4326, true}),
                        std::invalid_argument);
    }
  }
}