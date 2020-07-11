#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <meos/geos.hpp>

int main(int argc, char *argv[]) {
  // global setup...
  init_geos();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...
  finish_geos();

  return result;
}
