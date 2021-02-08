# Various SDK tools
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_BINARY_DIR}/src/HemeraCore
    ${CMAKE_BINARY_DIR}/src/HemeraQml
    ${CMAKE_BINARY_DIR}/src/HemeraTest
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/HemeraCore
    ${CMAKE_SOURCE_DIR}/src/HemeraQml
    ${CMAKE_SOURCE_DIR}/src/HemeraTest
)

add_subdirectory(hemera-application-gen)
add_subdirectory(hemera-application-launcher)
add_subdirectory(hemera-configure)
add_subdirectory(hemera-configuration-compiler)
add_subdirectory(hemera-debugscript-gen)
add_subdirectory(hemera-debug-helper)
add_subdirectory(hemera-properties-gen)

install(PROGRAMS verify-coverage-threshold.py
        DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}"
        COMPONENT Tools)
