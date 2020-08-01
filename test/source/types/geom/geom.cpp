#include <catch2/catch.hpp>
#include <meos/types/geom/GeomPoint.hpp>
#include <sstream>
#include <string>

using namespace meos;

TEST_CASE("Geometries are constructed and serialized properly", "[geometry]") {
  GeomPoint geometry;
  std::stringstream output;
  std::string expected = "POINT (2 3)";

  SECTION("<> constructor") { expected = "POINT (0 0)"; }

  SECTION("<double, double> constructor") { geometry = GeomPoint(2, 3); }

  SECTION("<string> constructor") {
    SECTION("WKT") { geometry = GeomPoint("POINT (2 3)"); }

    SECTION("WKB (Hex)") { geometry = GeomPoint("010100000000000000000000400000000000000840"); }
  }

  SECTION("<Geomety> constructor") {
    GeomPoint g(2, 3, 4326);
    geometry = GeomPoint(g);
    expected = "SRID=4326;POINT (2 3)";
  }

  SECTION("<double, double, int> constructor") {
    geometry = GeomPoint(2, 3, 4326);
    expected = "SRID=4326;POINT (2 3)";
  }

  SECTION("<string, int> constructor") {
    SECTION("SRID provided via geometry") {
      geometry = GeomPoint("SRID=4326;POINT (2 3)", 0);
      expected = "SRID=4326;POINT (2 3)";
    }

    SECTION("SRID provided in addition to geometry") {
      geometry = GeomPoint("POINT (2 3)", 4326);
      expected = "SRID=4326;POINT (2 3)";
    }

    SECTION("conflicting SRIDs provided") {
      REQUIRE_THROWS_AS((GeomPoint{"SRID=5676;POINT (2 3)", 4326}), std::invalid_argument);
      return;
    }
  }

  output << geometry;
  REQUIRE(output.str() == expected);
}

TEST_CASE("test read and write to WKB", "[geometry]") {
  GeomPoint g(2, 3, 4326);
  std::stringstream output;
  REQUIRE(g.srid() == 4326);
  g.toWKB(output);  // SRID would be lost in this process

  output.seekg(0);
  g.fromWKB(output);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("test read and write to WKT", "[geometry]") {
  GeomPoint g(2, 3, 4326);
  REQUIRE(g.srid() == 4326);
  std::string output = g.toWKT();  // SRID would be lost in this process

  g.fromWKT(output);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("test read and write to HEX", "[geometry]") {
  GeomPoint g(2, 3, 4326);
  std::stringstream output;
  REQUIRE(g.srid() == 4326);
  g.toHEX(output);  // SRID would be lost in this process

  output.seekg(0);
  g.fromHEX(output);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("fromHex", "[geometry]") {
  GeomPoint g;
  std::stringstream is("010100000000000000000000400000000000000840");
  g.fromHEX(is);
  REQUIRE(g.x() == 2);
  REQUIRE(g.y() == 3);
  REQUIRE(g.srid() == 0);
}

TEST_CASE("toHex", "[geometry]") {
  GeomPoint g(2, 3, 4326);
  std::stringstream output;
  g.toHEX(output);  // SRID would be lost in this process
  REQUIRE(output.str() == "010100000000000000000000400000000000000840");
}
