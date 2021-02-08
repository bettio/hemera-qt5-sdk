set(hemeracxxwrappercorelib_SRCS
    hemeracxxapplication.cpp
    hemeracxxapplicationproperties.cpp
    hemeracxxservicemanager.cpp

    hemeracxxproxyqt5application.cpp
    hemeracxxproxyqt5applicationproperties.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeracxxwrappercorelib_GENHEADERS
    Application
    ApplicationProperties
    Global
    ServiceManager
)

set(hemeracxxwrappercorelib_PVTHEADERS
    hemeracxxapplication_p.h

    hemeracxxproxyqt5application.h
    hemeracxxproxyqt5applicationproperties.h
)

hemera_qt_sdk_internal_generate_headers_prefix("hemeracxx" HemeraCxxCore hemeracxxwrappercorelib_HEADERS
                                               hemeracxxwrappercorelib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraCxxCore hemeracxxwrappercorelib_PVTHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(WrapperCore STATIC ${hemeracxxwrappercorelib_SRCS})
    target_link_libraries(WrapperCore gcov)
else ()
    add_library(WrapperCore SHARED ${hemeracxxwrappercorelib_SRCS})
endif ()

set_target_properties(WrapperCore PROPERTIES
                           OUTPUT_NAME HemeraCxxWrapperCore
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeracxxwrappercorelib_HEADERS}")
target_link_libraries(WrapperCore Core Qt5::Core Qt5::Network Qt5::DBus)

install(TARGETS WrapperCore
        EXPORT  HemeraCxxWrapperCoreTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraCxxWrapper/HemeraCxxCore"
        COMPONENT HemeraCxxWrapper)

# Export Core target
install(EXPORT HemeraCxxWrapperCoreTargets NAMESPACE HemeraCxxWrapper:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraCxxWrapperCore)

set(WRAPPERCORE_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraCxxWrapper"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraCxxWrapper/HemeraCxxCore"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraCore")
configure_file(HemeraCxxWrapperCoreConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCoreConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCoreConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCoreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCoreConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraCxxWrapperCore COMPONENT HemeraCxxWrapper)

# Pkgconfig
configure_file(HemeraCxxWrapperCore.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCore.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperCore.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraCxxWrapper)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeracxxwrappercorelib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraCxxWrapper/private/HemeraCxxCore"
            COMPONENT private)
endif ()
endif ()
