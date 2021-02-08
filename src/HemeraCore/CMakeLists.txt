set(hemeraqtsdkcorelib_SRCS
    hemeraappliancecrypto.cpp
    hemeraapplication.cpp
    hemeraapplicationproperties.cpp
    hemeraasyncinitobject.cpp
    hemeraasyncinitobject_p.h
    hemeraasyncinitdbusobject.cpp
    hemeraasyncinitdbusobject_p.h
    hemeracommonoperations.cpp
    hemeradbusapplicationholder.cpp
    hemeradevicemanagement.cpp
    hemeraheadlessapplication.cpp
    hemerafingerprints.cpp
    hemeragravity.cpp
    hemerainternalplanetinterface.cpp
    hemeranetworkaccessmanager.cpp
    hemeranetworkdownloadoperation.cpp
    hemeranetworktransparentoperation.cpp
    hemeranetworktransparentoperationclient.cpp
    hemeraoperation.cpp
    hemeraoperationwithprogress.cpp
    hemeraplanet.cpp
    hemeraprivateoperations_p.h
    hemeraqmldirnetworkreply.cpp
    hemeraremovablestorage.cpp
    hemerarootoperation.cpp
    hemerarootoperationclient.cpp
    hemeraservicemanager.cpp
    hemeraservicemodel.cpp
    hemerasystemconfig.cpp
    hemerausbgadgetmanager.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdkcorelib_GENHEADERS
    ApplianceCrypto
    Application
    ApplicationProperties
    AsyncInitObject
    AsyncInitDBusObject
    CommonOperations
    DeviceManagement
    Global
    HeadlessApplication
    Literals
    FetchSystemConfigOperation
    Fingerprints
    Gravity
    NetworkAccessManager
    NetworkDownloadOperation
    NetworkTransparentOperation
    NetworkTransparentOperationClient
    Operation
    OperationWithProgress
    Planet
    RemovableStorage
    RootOperation
    RootOperationClient
    ServiceManager
    ServiceModel
    SetSystemConfigOperation
    USBGadgetManager
)

set(hemeraqtsdkcorelib_PVTHEADERS
    hemeraapplication_p.h
    hemeraasyncinitdbusobject_p.h
    hemeraasyncinitobject_p.h
    hemeragravity_p.h
    hemeranetworktransparentoperation_p.h
    hemeranetworktransparentoperationclient_p.h
    hemeraoperation_p.h
    hemeraoperationwithprogress_p.h
    hemeraprivateoperations_p.h
    hemerarootoperation_p.h
    hemerarootoperationclient_p.h
    hemeraqmldirnetworkreply_p.h
)

hemera_qt_sdk_internal_generate_headers(HemeraCore hemeraqtsdkcorelib_HEADERS hemeraqtsdkcorelib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraCore hemeraqtsdkcorelib_PVTHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
# Add includes for DBus type marshalling
set_source_files_properties(${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.USBGadgetManager.xml PROPERTIES INCLUDE hemeraglobal.h)

qt5_add_dbus_interface(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Parsec.xml parsecinterface)
qt5_add_dbus_interface(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Parsec.ApplicationHandler.xml applicationhandlerinterface)
qt5_add_dbus_interface(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.DBusObject.xml dbusobjectinterface)
qt5_add_dbus_interface(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.RemovableStorageManager.xml removablestoragemanagerinterface)
qt5_add_dbus_interface(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.USBGadgetManager.xml usbgadgetmanagerinterface)

qt5_add_dbus_adaptor(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Application.xml
                     hemeraapplication.h Hemera::Application)
qt5_add_dbus_adaptor(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Application.DBusHolder.xml
                     hemeradbusapplicationholder_p.h Hemera::DBusApplicationHolder)
qt5_add_dbus_adaptor(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.DBusObject.xml
                     hemeraasyncinitdbusobject.h Hemera::AsyncInitDBusObject)
qt5_add_dbus_adaptor(hemeraqtsdkcorelib_SRCS ${CMAKE_SOURCE_DIR}/share/dbus/com.ispirata.Hemera.Planet.xml
                     hemerainternalplanetinterface_p.h Hemera::InternalPlanetInterface)

if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(Core STATIC ${hemeraqtsdkcorelib_SRCS})
    target_link_libraries(Core gcov)
else ()
    add_library(Core SHARED ${hemeraqtsdkcorelib_SRCS})
endif ()

set_target_properties(Core PROPERTIES
                           OUTPUT_NAME HemeraQt5SDKCore
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeraqtsdkcorelib_HEADERS}")
target_link_libraries(Core Qt5::Core Qt5::Network Qt5::DBus ${LIBSYSTEMD_DAEMON_LIBRARIES} ${LIBSYSTEMD_LOGIN_LIBRARIES})

install(TARGETS Core
        EXPORT  HemeraQt5SDKCoreTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraCore"
        COMPONENT HemeraQt5SDK)

# Export Core target
install(EXPORT HemeraQt5SDKCoreTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKCore)

set(CORE_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraCore")
configure_file(HemeraQt5SDKCoreConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCoreConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCoreConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCoreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCoreConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKCore COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKCore.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCore.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKCore.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeraqtsdkcorelib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/private/HemeraCore"
            COMPONENT private)
endif ()
endif ()

add_subdirectory(cxxwrapper)
