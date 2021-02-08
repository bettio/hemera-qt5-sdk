set(hemeraqtsdkguilib_SRCS
    hemeraguiapplication.cpp
    hemeraqmlguiapplication.cpp
    hemerasimpleqmlguiapplication.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdkguilib_GENHEADERS
    GuiApplication
    QmlGuiApplication
    SimpleQmlGuiApplication
)

set(hemeraqtsdkguilib_PVTHEADERS
    hemeraguiapplication_p.h
    hemeraqmlguiapplication_p.h
)

hemera_qt_sdk_internal_generate_headers(HemeraGui hemeraqtsdkguilib_HEADERS hemeraqtsdkguilib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraGui hemeraqtsdkguilib_PVTHEADERS)

# final libraries
if (NOT GENERATE_HEADERS_ONLY)
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(Gui STATIC ${hemeraqtsdkguilib_SRCS})
    target_link_libraries(Gui gcov)
else ()
    add_library(Gui SHARED ${hemeraqtsdkguilib_SRCS})
endif ()

set_target_properties(Gui PROPERTIES
                          OUTPUT_NAME HemeraQt5SDKGui
                          SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                          VERSION ${HEMERA_QT5_SDK_VERSION}
                          PUBLIC_HEADER "${hemeraqtsdkguilib_HEADERS}")
qt5_use_modules(Gui Core Network DBus Gui Quick)
target_link_libraries(Gui Core Qml)

# Install phase
install(TARGETS Gui
        EXPORT  HemeraQt5SDKGuiTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraGui"
        COMPONENT HemeraQt5SDK)

# Export Gui target
install(EXPORT HemeraQt5SDKGuiTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKGui)

set(GUI_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraGui")
configure_file(HemeraQt5SDKGuiConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGuiConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGuiConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGuiConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGuiConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKGui
        COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKGui.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGui.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGui.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeraqtsdkguilib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/private/HemeraGui"
            COMPONENT private)
endif ()
endif ()
