include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraCore
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraCore
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraTest
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraTest
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraGui
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraGui
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraQml
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraQml
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraWidgets
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraWidgets
                    ${CMAKE_BINARY_DIR}/include
)

# We are building the library
add_definitions(-DBUILDING_HEMERA_QT5_SDK)

# Config
configure_file(hemeractconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/hemeractconfig.h" @ONLY)

# Useful macros
macro(hemera_qt_sdk_internal_generate_headers_prefix _prefix _module _output _list)
    # This loop generates the headers
    foreach (f ${${_list}})
        string(TOLOWER ${f} gtmp)
        set(gen-header ${_prefix}${gtmp}.h)

        if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${gen-header}")
            message(FATAL_ERROR "The file ${f} does not have a corresponding ${gen-header} file. Aborting.")
        endif ()

        file(WRITE ${CMAKE_BINARY_DIR}/include/${_module}/${f} "#include \"${gen-header}\"\n")
        # Copy the file as well. needed in the next target
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${gen-header} DESTINATION ${CMAKE_BINARY_DIR}/include/${_module}/)

        list(APPEND ${_output} ${CMAKE_BINARY_DIR}/include/${_module}/${f} ${CMAKE_CURRENT_SOURCE_DIR}/${gen-header})
    endforeach (f)
endmacro()

macro(hemera_qt_sdk_internal_generate_headers _module _output _list)
    hemera_qt_sdk_internal_generate_headers_prefix("hemera" ${_module} ${_output} ${_list})
endmacro()

macro(hemera_qt_sdk_internal_generate_private_headers _module _list)
    foreach (f ${${_list}})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${f} DESTINATION ${CMAKE_BINARY_DIR}/include/private/${_module}/)
    endforeach (f)
endmacro()

# Modules
add_subdirectory(HemeraCore)
add_subdirectory(HemeraGenerators)
add_subdirectory(HemeraGui)
add_subdirectory(HemeraSoftwareManagement)
add_subdirectory(HemeraTest)
add_subdirectory(HemeraWidgets)
add_subdirectory(HemeraQml)

# Root operations
add_subdirectory(rootoperations)
