#include <catch2/catch.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <string>

TEST_CASE("geometries are serialized", "[serializer][geom]") {
  Serializer<Geometry> w;
  int x = GENERATE(take(10, random(-1000, 1000)));
  int y = GENERATE(take(10, random(-1000, 1000)));
  string wkt = "POINT (" + to_string(x) + " " + to_string(y) + ")";

  SECTION("Without SRID") {
    Geometry g(x, y);
    REQUIRE(g.geom != nullptr);
    REQUIRE(w.write(g) == wkt);
  }

  SECTION("With SRID") {
    Geometry g(x, y, 4326);
    REQUIRE(g.geom != nullptr);
    REQUIRE(w.write(g) == "SRID=4326;" + wkt);
  }
}

TEST_CASE("geometries are deserialized", "[deserializer][geom]") {
  double expectedX = GENERATE(take(10, random(-100000, 100000))) / 1000.0;
  double expectedY = GENERATE(take(10, random(-100000, 100000))) / 1000.0;
  double x, y;
  int expected_srid = 0;
  Geometry g;

  SECTION("Without SRID") {
    string serialized = "POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    Deserializer<Geometry> r(serialized);

    g = r.nextValue();
    CHECK_THROWS(r.nextValue());
  }

  SECTION("With SRID") {
    expected_srid = 4326;
    string serialized
        = "SRID=4326;POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    Deserializer<Geometry> r(serialized);

    g = r.nextValue();
    CHECK_THROWS(r.nextValue());
  }

  SECTION("spaces within geometry shouldn't affect") {
    char buffer[256];

    // clang-format off
    char const *fmt = GENERATE(
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

    g = r.nextValue();
    CHECK_THROWS(r.nextValue());
  }

  SECTION("ending with @ should work") {
    expected_srid = 4326;
    string serialized
        = "SRID=4326;POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")@";
    Deserializer<Geometry> r(serialized);

    g = r.nextValue();
    CHECK_THROWS(r.nextValue());
  }

  GEOSGeomGetX_r(geos_context, g.geom, &x);
  GEOSGeomGetY_r(geos_context, g.geom, &y);
  REQUIRE(x == expectedX);
  REQUIRE(y == expectedY);
  REQUIRE(g.srid() == expected_srid);
}

TEST_CASE("geometry serdes", "[serializer][deserializer][geom]") {
  Serializer<Geometry> w;
  SECTION("only one geometry present") {
    int expectedX = GENERATE(take(10, random(-1000, 1000)));
    int expectedY = GENERATE(take(10, random(-1000, 1000)));
    string expected_serialized
        = "POINT (" + to_string(expectedX) + " " + to_string(expectedY) + ")";
    Geometry g(expectedX, expectedY);
    string serialized = w.write(g);
    REQUIRE(serialized == expected_serialized);
    Deserializer<Geometry> r(serialized);

    g = r.nextValue();
    REQUIRE(g.geom != nullptr);
    double x, y;
    GEOSGeomGetX_r(geos_context, g.geom, &x);
    GEOSGeomGetY_r(geos_context, g.geom, &y);
    REQUIRE(x == expectedX);
    REQUIRE(y == expectedY);
  }
}
