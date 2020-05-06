#include "../catch.hpp"
#include <iostream>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>

void notice(const char *fmt, ...) {}

TEST_CASE("geometries are serialized", "[serializer][geom]") {
  initGEOS(notice, notice);
  Serializer<Geometry> w;
  SECTION("only one geometry present") {
    int x = GENERATE(take(10, random(-1000, 1000)));
    int y = GENERATE(take(10, random(-1000, 1000)));
    string serialized = "POINT (" + to_string(x) + " " + to_string(y) + ")";
    Geometry g(x, y);
    REQUIRE(g.geom != nullptr);
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
    Deserializer<Geometry> r(serialized);

    Geometry g = r.nextValue();
    double x, y;
    GEOSGeomGetX(g.geom, &x);
    GEOSGeomGetY(g.geom, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);

    CHECK_THROWS(r.nextValue());
  }
  SECTION("spaces within geometry shouldn't affect") {
    double expectedX = GENERATE(take(5, random(-100000, 100000))) / 1000.0;
    double expectedY = GENERATE(take(5, random(-100000, 100000))) / 1000.0;
    char buffer[256];

    // clang-format off
    const char *fmt = GENERATE(
      "POINT (%lf %lf)",
      "POINT(%lf %lf)",
      "POINT(%lf    %lf)",
      "POINT (%lf    %lf)",
      "POINT (  %lf    %lf    )"
    );
    // clang-format on

    sprintf(buffer, fmt, expectedX, expectedY);
    string serialized = buffer;
    Deserializer<Geometry> r(serialized);

    Geometry g = r.nextValue();
    double x, y;
    GEOSGeomGetX(g.geom, &x);
    GEOSGeomGetY(g.geom, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);

    CHECK_THROWS(r.nextValue());
  }
  finishGEOS();
}

TEST_CASE("geometry serdes", "[serializer][deserializer][geom]") {
  initGEOS(notice, notice);
  Serializer<Geometry> w;
  SECTION("only one geometry present") {
    int expectedX = GENERATE(take(10, random(-1000, 1000)));
    int expectedY = GENERATE(take(10, random(-1000, 1000)));
    string expected_serialized =
        "POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    Geometry g(expectedX, expectedY);
    string serialized = w.write(g);
    REQUIRE(serialized == expected_serialized);
    Deserializer<Geometry> r(serialized);

    g = r.nextValue();
    REQUIRE(g.geom != nullptr);
    double x, y;
    GEOSGeomGetX(g.geom, &x);
    GEOSGeomGetY(g.geom, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);
  }
  finishGEOS();
}
