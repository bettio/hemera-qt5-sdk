set(hemeradebugscriptgen_SRCS
    main.cpp
    debugscriptgenerator.cpp
)

# final libraries
add_executable(hemera-debug-script-gen ${hemeradebugscriptgen_SRCS})
qt5_use_modules(hemera-debug-script-gen Core Network DBus Qml)
target_link_libraries(hemera-debug-script-gen Core Qml)


# Install phase
install(TARGETS hemera-debug-script-gen
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
