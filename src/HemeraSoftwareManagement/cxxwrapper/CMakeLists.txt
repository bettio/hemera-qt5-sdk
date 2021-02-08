set(hemeracxxwrappersoftwaremanagementlib_SRCS
    hemeracxxsoftwaremanagementapplicationpackage.cpp
    hemeracxxsoftwaremanagementapplicationupdate.cpp
    hemeracxxsoftwaremanagementrepository.cpp
    #hemeracxxsoftwaremanagementsoftwaremanager.cpp
    hemeracxxsoftwaremanagementsystemupdate.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeracxxwrappersoftwaremanagementlib_GENHEADERS
    ApplicationPackage
    ApplicationUpdate
    Repository
    SoftwareManager
    SystemUpdate
)

set(hemeracxxwrappersoftwaremanagementlib_PVTHEADERS
    hemeracxxsoftwaremanagementapplicationpackage_p.h
    hemeracxxsoftwaremanagementapplicationupdate_p.h
)

hemera_qt_sdk_internal_generate_headers_prefix("hemeracxxsoftwaremanagement" HemeraCxxSoftwareManagement hemeracxxwrappersoftwaremanagementlib_HEADERS
                                               hemeracxxwrappersoftwaremanagementlib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraCxxSoftwareManagement hemeracxxwrappersoftwaremanagementlib_PVTHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(WrapperSoftwareManagement STATIC ${hemeracxxwrappersoftwaremanagementlib_SRCS})
    target_link_libraries(WrapperSoftwareManagement gcov)
else ()
    add_library(WrapperSoftwareManagement SHARED ${hemeracxxwrappersoftwaremanagementlib_SRCS})
endif ()

set_target_properties(WrapperSoftwareManagement PROPERTIES
                           OUTPUT_NAME HemeraCxxWrapperSoftwareManagement
                           SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                           VERSION ${HEMERA_QT5_SDK_VERSION}
                           PUBLIC_HEADER "${hemeracxxwrappersoftwaremanagementlib_HEADERS}")
target_link_libraries(WrapperSoftwareManagement SoftwareManagement WrapperCore Qt5::Core Qt5::Network Qt5::DBus)

install(TARGETS WrapperSoftwareManagement
        EXPORT  HemeraCxxWrapperSoftwareManagementTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraCxxWrapper/HemeraCxxSoftwareManagement"
        COMPONENT HemeraCxxWrapper)

# Export SoftwareManagement target
install(EXPORT HemeraCxxWrapperSoftwareManagementTargets NAMESPACE HemeraCxxWrapper:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraCxxWrapperSoftwareManagement)

set(WRAPPERSOFTWAREMANAGEMENT_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraCxxWrapper"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraCxxWrapper/HemeraCxxSoftwareManagement"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK"
                             "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraSoftwareManagement")
configure_file(HemeraCxxWrapperSoftwareManagementConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagementConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagementConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagementConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagementConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraCxxWrapperSoftwareManagement COMPONENT HemeraCxxWrapper)

# Pkgconfig
configure_file(HemeraCxxWrapperSoftwareManagement.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagement.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraCxxWrapperSoftwareManagement.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraCxxWrapper)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeracxxwrappersoftwaremanagementlib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraCxxWrapper/private/HemeraCxxSoftwareManagement"
            COMPONENT private)
endif ()
endif ()
