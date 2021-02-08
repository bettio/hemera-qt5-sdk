set(hemeraqtsdksoftwaremanagementlib_SRCS
    hemerasoftwaremanagementappliancemanager.cpp
    hemerasoftwaremanagementapplicationmanager.cpp
    hemerasoftwaremanagementapplicationpackage.cpp
    hemerasoftwaremanagementapplicationupdate.cpp
    hemerasoftwaremanagementprogressreporter.cpp
    hemerasoftwaremanagementrepository.cpp
    hemerasoftwaremanagementsystemupdate.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdksoftwaremanagementlib_GENHEADERS
    ApplianceManager
    ApplicationManager
    ApplicationPackage
    ApplicationUpdate
    ProgressReporter
    Repository
    SystemUpdate
)

set(hemeraqtsdksoftwaremanagementlib_PVTHEADERS
    hemerasoftwaremanagementconstructors_p.h
)

hemera_qt_sdk_internal_generate_headers_prefix("hemerasoftwaremanagement" HemeraSoftwareManagement hemeraqtsdksoftwaremanagementlib_HEADERS
                                               hemeraqtsdksoftwaremanagementlib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraSoftwareManagement hemeraqtsdksoftwaremanagementlib_PVTHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
qt5_add_dbus_interface(hemeraqtsdksoftwaremanagementlib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.SoftwareManagement.ApplianceManager.xml appliancemanagerinterface)
qt5_add_dbus_interface(hemeraqtsdksoftwaremanagementlib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.SoftwareManagement.ApplicationManager.xml applicationmanagerinterface)
qt5_add_dbus_interface(hemeraqtsdksoftwaremanagementlib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.SoftwareManagement.ProgressReporter.xml progressreporterinterface)

qt5_add_dbus_adaptor(hemeraqtsdksoftwaremanagementlib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.DBusObject.xml
                     hemeraasyncinitdbusobject.h Hemera::AsyncInitDBusObject)

if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(SoftwareManagement STATIC ${hemeraqtsdksoftwaremanagementlib_SRCS})
    target_link_libraries(SoftwareManagement gcov)
else ()
    add_library(SoftwareManagement SHARED ${hemeraqtsdksoftwaremanagementlib_SRCS})
endif ()

set_target_properties(SoftwareManagement PROPERTIES
                           OUTPUT_NAME HemeraQt5SDKSoftwareManagement
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeraqtsdksoftwaremanagementlib_HEADERS}")
target_link_libraries(SoftwareManagement Core Qt5::Core Qt5::Network Qt5::DBus ${LIBSYSTEMD_DAEMON_LIBRARIES} ${LIBSYSTEMD_LOGIN_LIBRARIES})

install(TARGETS SoftwareManagement
        EXPORT  HemeraQt5SDKSoftwareManagementTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraSoftwareManagement"
        COMPONENT HemeraQt5SDK)

# Export SoftwareManagement target
install(EXPORT HemeraQt5SDKSoftwareManagementTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKSoftwareManagement)

set(SOFTWAREMANAGEMENT_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraSoftwareManagement")
configure_file(HemeraQt5SDKSoftwareManagementConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagementConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagementConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagementConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagementConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKSoftwareManagement COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKSoftwareManagement.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagement.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKSoftwareManagement.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeraqtsdksoftwaremanagementlib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/private/HemeraSoftwareManagement"
            COMPONENT private)
endif ()
endif ()

add_subdirectory(cxxwrapper)
