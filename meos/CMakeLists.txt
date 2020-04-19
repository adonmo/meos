file(GLOB_RECURSE _sources ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_library(meos ${_sources})

target_include_directories(meos PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
