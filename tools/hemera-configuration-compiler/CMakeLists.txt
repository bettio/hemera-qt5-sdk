set(hemeraconfigurationcompiler_SRCS
    main.cpp
    compiler.cpp)

# final libraries
add_executable(hemera-configuration-compiler ${hemeraconfigurationcompiler_SRCS})
qt5_use_modules(hemera-configuration-compiler Core Network DBus Qml)
target_link_libraries(hemera-configuration-compiler Core Qml Generators)


# Install phase
install(TARGETS hemera-configuration-compiler
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
