set(hemeraconfigure_SRCS
    main.cpp
    configure.cpp)

# final libraries
add_executable(hemera-configure ${hemeraconfigure_SRCS})
qt5_use_modules(hemera-configure Core Network DBus Qml)
target_link_libraries(hemera-configure Core Qml Generators)


# Install phase
install(TARGETS hemera-configure
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
