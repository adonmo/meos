cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(
  libmeos-tests
  LANGUAGES CXX
)

get_filename_component(MEOS_ROOT ../ ABSOLUTE)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# ---- Dependencies ----

include("${MEOS_ROOT}/cmake/CPM.cmake")

CPMAddPackage(
  NAME Catch2
  GITHUB_REPOSITORY catchorg/Catch2
  VERSION 2.12.2
)

if (TEST_INSTALLED_VERSION)
  find_package(libmeos REQUIRED)
else()
  CPMAddPackage(
    NAME libmeos
    SOURCE_DIR "${MEOS_ROOT}"
  )
endif()

CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.3
)

# ---- Create binary ----

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
add_executable(libmeos-tests ${sources})
target_link_libraries(libmeos-tests libmeos Catch2)

set_target_properties(libmeos-tests PROPERTIES CXX_STANDARD 14)

# enable compiler warnings
if (NOT TEST_INSTALLED_VERSION)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(libmeos PUBLIC)
  elseif(MSVC)
    target_compile_options(libmeos PUBLIC /W4 /WX)
  endif()
endif()

# ---- Add libmeos-tests ----

ENABLE_TESTING()

add_test(NAME libmeos COMMAND libmeos-tests)

# ---- code coverage ----

if (ENABLE_TEST_COVERAGE)
  target_compile_options(libmeos PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(libmeos PUBLIC -fprofile-arcs -ftest-coverage)
endif()
