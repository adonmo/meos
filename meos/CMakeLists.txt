file(GLOB_RECURSE _sources ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_library(meos ${_sources})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include libgeos C++ API
find_package(GEOS REQUIRED)
execute_process(COMMAND geos-config --clibs OUTPUT_VARIABLE GEOS_CLIBS)
string(STRIP "${GEOS_CLIBS}" GEOS_CLIBS)
target_link_libraries(meos "${GEOS_CLIBS}")

target_include_directories(meos PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
