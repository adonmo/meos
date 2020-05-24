add_custom_target (
        write_version_info
        COMMAND python ${CMAKE_SOURCE_DIR}/tools/write_version_info.py
                       ${CMAKE_BINARY_DIR}/meos_version_info.h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

include_directories(${CMAKE_BINARY_DIR})

execute_process(COMMAND python tools/print_version.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE MEOS_VERSION)

message(${MEOS_VERSION})
