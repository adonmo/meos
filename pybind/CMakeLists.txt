file(GLOB_RECURSE _sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

find_package(pybind11)
pybind11_add_module(libmeos ${_sources})
target_link_libraries(libmeos PRIVATE meos)
set_target_properties(libmeos PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
