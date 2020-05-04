cmake_minimum_required(VERSION 3.10)
project(libmeos VERSION 0.0.2)
SET(BUILD_SHARED_LIBS ON)

enable_testing()

add_subdirectory(meos)
add_subdirectory(tests)
add_subdirectory(pybind)
