set(hemeraqtsdktestlib_SRCS
    hemeradbustest.cpp
    hemerafakeheadlessapplication.cpp
    hemerafakegravitycenter.cpp
    hemerafakeparsec.cpp
    hemeratest.cpp
    hemeratestwithapplicationenvironment.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdktestlib_GENHEADERS
    DBusTest
    FakeHeadlessApplication
    FakeGravityCenter
    FakeParsec
    Test
    TestWithApplicationEnvironment
)

# This loop generates the headers
hemera_qt_sdk_internal_generate_headers(HemeraTest hemeraqtsdktestlib_HEADERS hemeraqtsdktestlib_GENHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
# qt5_add_dbus_adaptor(hemeraqtsdktestlib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Parsec.xml
#                      hemerafakegravitycenter.h Hemera::Test::FakeGravityCenter)

add_library(Test SHARED ${hemeraqtsdktestlib_SRCS})
target_link_libraries(Test Core)
qt5_use_modules(Test Core DBus Test)

set_target_properties(Test PROPERTIES
                           OUTPUT_NAME HemeraQt5SDKTest
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeraqtsdktestlib_HEADERS}")

# Fake applications
add_executable(hemera-fake-headless-application hemerafakeheadlessapplication.cpp)
target_link_libraries(hemera-fake-headless-application Test Core)
qt5_use_modules(hemera-fake-headless-application Core DBus Test)
set_target_properties(hemera-fake-headless-application PROPERTIES COMPILE_DEFINITIONS "BUILD_AS_STANDALONE_APPS")

add_executable(hemera-fake-parsec hemerafakegravitycenter.cpp)
target_link_libraries(hemera-fake-parsec Test Core)
qt5_use_modules(hemera-fake-parsec Core DBus Test)
set_target_properties(hemera-fake-parsec PROPERTIES COMPILE_DEFINITIONS "BUILD_AS_STANDALONE_APPS")

# Install phase
install(TARGETS Test
        EXPORT  HemeraQt5SDKTestTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraTest"
        COMPONENT HemeraQt5SDK)

# Export Test target
install(EXPORT HemeraQt5SDKTestTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKTest)

set(TEST_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraTest")
configure_file(HemeraQt5SDKTestConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTestConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTestConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTestConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTestConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKTest COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKTest.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTest.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKTest.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)
endif ()
