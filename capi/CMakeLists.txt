cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(
  meos_c
  LANGUAGES C CXX
)
# Get version from VERSION file instead of hardcoding here
# This is so that we can maintain a single version across C/C++ and Python
file(STRINGS ../VERSION LIBMEOS_VERSION)
set(PROJECT_VERSION ${LIBMEOS_VERSION})
message(STATUS "MEOS: v${LIBMEOS_VERSION}")

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME libmeos
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
)

# ---- Create library ----

file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
add_library(meos_c SHARED ${headers} ${sources})
target_link_libraries(meos_c libmeos)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(meos_c PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

target_include_directories(meos_c
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

message(${PROJECT_SOURCE_DIR}/include)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  DEPENDENCIES ""
)
