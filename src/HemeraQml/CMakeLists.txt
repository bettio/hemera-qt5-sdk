set(hemeraqtsdkqmllib_SRCS
    hemeraqmlengine.cpp
    hemeraqmlnetworkaccessmanagerfactory.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdkqmllib_GENHEADERS
    QmlEngine
    QmlNetworkAccessManagerFactory
)

# This loop generates the headers
hemera_qt_sdk_internal_generate_headers(HemeraQml hemeraqtsdkqmllib_HEADERS hemeraqtsdkqmllib_GENHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
add_library(Qml SHARED ${hemeraqtsdkqmllib_SRCS})
target_link_libraries(Qml Core)
qt5_use_modules(Qml Core DBus Qml)

set_target_properties(Qml PROPERTIES
                           OUTPUT_NAME HemeraQt5SDKQml
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeraqtsdkqmllib_HEADERS}")

# Install phase
install(TARGETS Qml
        EXPORT  HemeraQt5SDKQmlTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraQml"
        COMPONENT HemeraQt5SDK)

# Export Qml target
install(EXPORT HemeraQt5SDKQmlTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKQml)

set(QML_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraQml")
configure_file(HemeraQt5SDKQmlConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQmlConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQmlConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQmlConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQmlConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKQml COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKQml.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQml.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKQml.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)
endif ()
