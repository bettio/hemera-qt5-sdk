set(hemerapropertiesgen_SRCS
    main.cpp
    propertiesgenerator.cpp
)

# final libraries
add_executable(hemera-properties-gen ${hemerapropertiesgen_SRCS})
qt5_use_modules(hemera-properties-gen Core Network DBus Qml)
target_link_libraries(hemera-properties-gen Core Qml Generators)


# Install phase
install(TARGETS hemera-properties-gen
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
