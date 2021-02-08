set(hemeraqtsdkwidgetslib_SRCS
    hemeradeclarativeengine.cpp
    hemeradeclarativenetworkaccessmanagerfactory.cpp
    hemeraqtquick1application.cpp
    hemerasimpleqtquick1application.cpp
    hemerawidgetsapplication.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdkwidgetslib_GENHEADERS
    DeclarativeEngine
    DeclarativeNetworkAccessManagerFactory
    QtQuick1Application
    SimpleQtQuick1Application
    WidgetsApplication
)

set(hemeraqtsdkwidgetslib_PVTHEADERS
    hemeraqtquick1application_p.h
    hemerawidgetsapplication_p.h
)

hemera_qt_sdk_internal_generate_headers(HemeraWidgets hemeraqtsdkwidgetslib_HEADERS hemeraqtsdkwidgetslib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraWidgets hemeraqtsdkwidgetslib_PVTHEADERS)

# final libraries
if (NOT GENERATE_HEADERS_ONLY)
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(Widgets STATIC ${hemeraqtsdkwidgetslib_SRCS})
    target_link_libraries(Widgets gcov)
else ()
    add_library(Widgets SHARED ${hemeraqtsdkwidgetslib_SRCS})
endif ()

set_target_properties(Widgets PROPERTIES
                              OUTPUT_NAME HemeraQt5SDKWidgets
                              SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                              VERSION ${HEMERA_QT5_SDK_VERSION}
                              PUBLIC_HEADER "${hemeraqtsdkwidgetslib_HEADERS}")
qt5_use_modules(Widgets Core Network DBus Widgets Declarative)
target_link_libraries(Widgets Core)

# Install phase
install(TARGETS Widgets
        EXPORT  HemeraQt5SDKWidgetsTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraWidgets"
        COMPONENT HemeraQt5SDK)

# Export Widgets target
install(EXPORT HemeraQt5SDKWidgetsTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKWidgets)

set(WIDGETS_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraWidgets")
configure_file(HemeraQt5SDKWidgetsConfig.cmake.in
               "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HemeraQt5SDKWidgetsConfig.cmake" @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
              "${CMAKE_BINARY_DIR}/HemeraQt5SDKWidgetsConfigVersion.cmake" @ONLY)

install(FILES
        "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HemeraQt5SDKWidgetsConfig.cmake"
        "${CMAKE_BINARY_DIR}/HemeraQt5SDKWidgetsConfigVersion.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}/HemeraQt5SDKWidgets"
        COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKWidgets.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKWidgets.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKWidgets.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeraqtsdkwidgetslib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/private/HemeraWidgets"
            COMPONENT private)
endif ()
endif ()
