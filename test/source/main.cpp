#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <meos/geos.hpp>

int main(int argc, char *argv[]) {
  // global setup...
  meos::init_geos();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...
  meos::finish_geos();

  return result;
}
