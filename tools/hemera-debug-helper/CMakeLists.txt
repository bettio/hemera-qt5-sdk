set(hemeradebughelper_SRCS
    debughelper.cpp
    main.cpp
)

# final libraries
add_executable(hemera-debug-helper ${hemeradebughelper_SRCS})
qt5_use_modules(hemera-debug-helper Core Network DBus)
target_link_libraries(hemera-debug-helper Core)


# Install phase
install(TARGETS hemera-debug-helper
        RUNTIME DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_APPLICATION_TOOLS_DIR}" COMPONENT shlib
        COMPONENT Tools)
