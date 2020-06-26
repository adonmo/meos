cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(
  meos_docs
)

get_filename_component(MEOS_ROOT ../ ABSOLUTE)

# ---- Dependencies ----

include("${MEOS_ROOT}/cmake/CPM.cmake")

CPMAddPackage(
  NAME libdaylight
  SOURCE_DIR "${MEOS_ROOT}"
)

CPMAddPackage(
  NAME StableCoder-cmake-scripts
  GITHUB_REPOSITORY StableCoder/cmake-scripts
  GIT_TAG 3d2d5a9fb26f0ce24e3e4eaeeff686ec2ecfb3fb
)

# ---- Doxygen ----

set(BUILD_DOCUMENTATION ON CACHE INTERNAL "")
include(${StableCoder-cmake-scripts_SOURCE_DIR}/doxygen.cmake)

# set Doxyfile variables
set(DOXYGEN_PROJECT_NAME daylight)
set(DOXYGEN_PROJECT_VERSION ${daylight_VERSION})
set(DOXYGEN_PROJECT_ROOT "${MEOS_ROOT}")

# see https://github.com/StableCoder/cmake-scripts#doxygen-doxygencmake for additional options
build_docs(
  TARGET_NAME cppdocs
  DOXYFILE_PATH ${CMAKE_CURRENT_LIST_DIR}/cpp/Doxyfile
  OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen
  PROCESS_DOXYFILE ON
)
