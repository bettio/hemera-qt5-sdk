set(hemeraapplicationgen_SRCS
    main.cpp
    applicationgenerator.cpp
)

# final libraries
add_executable(hemera-application-gen ${hemeraapplicationgen_SRCS})
qt5_use_modules(hemera-application-gen Core Network DBus Qml)
target_link_libraries(hemera-application-gen Core Qml Generators)


# Install phase
install(TARGETS hemera-application-gen
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
