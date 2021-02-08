set(hemeraapplicationlauncher_SRCS
    main.cpp
    applicationlauncherproperties.cpp
    hemeraapplicationlauncher.cpp
)

# final libraries
add_executable(hemera-application-launcher ${hemeraapplicationlauncher_SRCS})
qt5_use_modules(hemera-application-launcher Core Network DBus Qml)
target_link_libraries(hemera-application-launcher Core Qml Generators)


# Install phase
install(TARGETS hemera-application-launcher
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
