include_directories(
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_BINARY_DIR}/src/HemeraCore
    ${CMAKE_BINARY_DIR}/src/HemeraTest
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/HemeraCore
    ${CMAKE_SOURCE_DIR}/src/HemeraTest
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

if (ENABLE_HEMERA_QT5_SDK_TESTS)
    #configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/tests-config.h )

    add_subdirectory(dbus-1)

    hemera_setup_dbus_test_environment()

    hemera_add_unit_test(AsyncInitObjectBasics async-init-object-basics)
    hemera_add_dbus_unit_test(AsyncInitDBusObjectBasics async-init-dbus-object-basics)
    hemera_add_dbus_unit_test(CommonOperationsBasics common-operations-basics)
#     hemera_add_dbus_unit_test(GravityBasics gravity-center-basics)
    hemera_add_unit_test(OperationBasics operation-basics)

    # Some tests work only if test codepaths have been enabled. As such, disable builds if codepaths have been deactivated.
#     if (ENABLE_HEMERA_QT5_SDK_TEST_CODEPATHS)
#         hemera_add_dbus_unit_test(HeadlessApplicationBasics headless-application-basics)
#         hemera_add_dbus_unit_test(NetworkAccessManagerBasics network-access-manager-basics)
#     endif (ENABLE_HEMERA_QT5_SDK_TEST_CODEPATHS)

endif (ENABLE_HEMERA_QT5_SDK_TESTS)

# This should always be built and installed for packaging reasons

