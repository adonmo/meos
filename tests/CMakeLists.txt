file(GLOB_RECURSE _sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_executable(meos-tests ${_sources})

target_link_libraries(meos-tests PRIVATE meos)

enable_testing()
add_test(NAME meos COMMAND meos-tests)
