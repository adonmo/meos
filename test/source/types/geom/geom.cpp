#include <catch2/catch.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <sstream>
#include <string>

TEST_CASE("Geometries are constructed and serialized properly", "[geometry]") {
  Geometry geometry;
  std::stringstream output;
  std::string expected = "POINT (2 3)";

  SECTION("<> constructor") { expected = "POINT (0 0)"; }

  SECTION("<double, double> constructor") { geometry = Geometry(2, 3); }

  SECTION("<string> constructor") { geometry = Geometry("POINT (2 3)"); }

  SECTION("<Geomety> constructor") {
    Geometry g(2, 3, 4326);
    geometry = Geometry(g);
    expected = "SRID=4326;POINT (2 3)";
  }

  SECTION("<double, double, int> constructor") {
    geometry = Geometry(2, 3, 4326);
    expected = "SRID=4326;POINT (2 3)";
  }

  SECTION("<string, int> constructor") {
    SECTION("SRID provided via geometry") {
      geometry = Geometry("SRID=4326;POINT (2 3)", 0);
      expected = "SRID=4326;POINT (2 3)";
    }

    SECTION("SRID provided in addition to geometry") {
      geometry = Geometry("POINT (2 3)", 4326);
      expected = "SRID=4326;POINT (2 3)";
    }

    SECTION("conflicting SRIDs provided") {
      REQUIRE_THROWS_AS((Geometry{"SRID=5676;POINT (2 3)", 4326}), std::invalid_argument);
      return;
    }
  }

  output << geometry;
  REQUIRE(output.str() == expected);
}

TEST_CASE("test read and write to WKB", "[geometry]") {
  Geometry g(2, 3, 4326);
  std::stringstream output;
  REQUIRE(g.srid() == 4326);
  g.toWKB(output);  // SRID would be lost in this process

  output.seekg(0);
  g.fromWKB(output);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("fromHex", "[geometry]") {
  Geometry g;
  std::stringstream is("010100000000000000000000400000000000000840");
  g.fromHEX(is);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("toHex", "[geometry]") {
  Geometry g(2, 3, 4326);
  std::stringstream output;
  g.toHEX(output);  // SRID would be lost in this process
  REQUIRE(output.str() == "010100000000000000000000400000000000000840");
}
