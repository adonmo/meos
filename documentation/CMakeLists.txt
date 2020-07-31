cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(
  meos_docs
)

get_filename_component(MEOS_ROOT ../ ABSOLUTE)

# ---- Dependencies ----

include("${MEOS_ROOT}/cmake/CPM.cmake")

CPMAddPackage(
  NAME libmeos
  SOURCE_DIR "${MEOS_ROOT}"
)

CPMAddPackage(
  NAME MCSS
  GITHUB_REPOSITORY TheLartians/m.css
  # using the patch until https://github.com/mosra/m.css/pull/171 is resolved
  GIT_TAG 1bf162b96d5bfefc9967a80cef138f1270ffa415
  DOWNLOAD_ONLY YES
)

# ---- Setup targets for python docs ----

add_custom_target(
    pydocs
    make html
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/python"
)

add_custom_target(
    pydoctest
    make doctest
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/python"
)

# ---- Doxygen ----

# set Doxyfile variables
set(DOXYGEN_PROJECT_NAME meos)
set(DOXYGEN_PROJECT_VERSION ${meos_VERSION})
set(DOXYGEN_PROJECT_ROOT "${MEOS_ROOT}")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cppdocs")

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cpp/Doxyfile
  ${CMAKE_CURRENT_BINARY_DIR}/cpp/Doxyfile
)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cpp/conf.py
  ${CMAKE_CURRENT_BINARY_DIR}/cpp/conf.py
)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cpp/favicon-dark.png
  ${CMAKE_CURRENT_BINARY_DIR}/cpp/favicon-dark.png
  COPYONLY
)

add_custom_target(
  cppdocs
  ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
  COMMAND "${MCSS_SOURCE_DIR}/documentation/doxygen.py" "${CMAKE_CURRENT_BINARY_DIR}/cpp/conf.py"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
