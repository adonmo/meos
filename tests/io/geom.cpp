#include "../catch.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>

void notice(const char *fmt, ...) {}

TEST_CASE("geometries are serialized", "[serializer][geom]") {
  initGEOS(notice, notice);
  Serializer<GEOSGeometry *> w;
  SECTION("only one geometry present") {
    int x = GENERATE(take(10, random(-1000, 1000)));
    int y = GENERATE(take(10, random(-1000, 1000)));
    string serialized = "POINT (" + to_string(x) + " " + to_string(y) + ")";
    GEOSGeometry *g = GEOSGeomFromWKT(serialized.c_str());
    REQUIRE(g != nullptr);
    REQUIRE(w.write(g) == serialized);
  }
  finishGEOS();
}

TEST_CASE("geometries are deserialized", "[deserializer][geom]") {
  initGEOS(notice, notice);
  SECTION("only one geometry present") {
    double expectedX = GENERATE(take(10, random(-100000, 100000))) / 1000.0;
    double expectedY = GENERATE(take(10, random(-100000, 100000))) / 1000.0;
    string serialized =
        "POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    Deserializer<GEOSGeometry *> r(serialized);

    GEOSGeometry *g = r.nextValue();
    double x, y;
    GEOSGeomGetX(g, &x);
    GEOSGeomGetY(g, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);

    CHECK_THROWS(r.nextValue());
  }
  finishGEOS();
}

TEST_CASE("geometry serdes", "[serializer][deserializer][geom]") {
  initGEOS(notice, notice);
  Serializer<GEOSGeometry *> w;
  SECTION("only one geometry present") {
    int expectedX = GENERATE(take(10, random(-1000, 1000)));
    int expectedY = GENERATE(take(10, random(-1000, 1000)));
    string serialized =
        "POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    GEOSGeometry *g = GEOSGeomFromWKT(serialized.c_str());
    Deserializer<GEOSGeometry *> r(w.write(g));

    g = r.nextValue();
    double x, y;
    GEOSGeomGetX(g, &x);
    GEOSGeomGetY(g, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);
  }
  finishGEOS();
}
