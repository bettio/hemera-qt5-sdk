set(hemeraqtsdkgeneratorslib_SRCS
    hemerageneratorsbasecompiler.cpp
    hemerageneratorsbaseconfigure.cpp
    hemerageneratorsbasegenerator.cpp
    hemerageneratorsbasehagenerator.cpp
    hemerageneratorsbasepropertiesgenerator.cpp
    hemerageneratorscompilerplugin.cpp
    hemerageneratorsconfigureplugin.cpp
    hemerageneratorsgeneratorplugin.cpp
    hemerageneratorspropertiesplugin.cpp
)

# Register here all the generated uppercase headers. The lowercase ones matching ones in this list will be installed as well
set(hemeraqtsdkgeneratorslib_GENHEADERS
    BaseCompiler
    BaseConfigure
    BaseGenerator
    BaseHaGenerator
    BasePropertiesGenerator
    GeneratorPluginManager

    CompilerPlugin
    ConfigurePlugin
    GeneratorPlugin
    PropertiesPlugin
)

set(hemeraqtsdkgeneratorslib_PVTHEADERS
    hemerageneratorsbasegenerator_p.h
    hemerageneratorsbasehagenerator_p.h

    hemerageneratorscompilerplugin_p.h
    hemerageneratorsconfigureplugin_p.h
    hemerageneratorsgeneratorplugin_p.h
    hemerageneratorspropertiesplugin_p.h
)

hemera_qt_sdk_internal_generate_headers_prefix("hemeragenerators" HemeraGenerators hemeraqtsdkgeneratorslib_HEADERS
                                               hemeraqtsdkgeneratorslib_GENHEADERS)
hemera_qt_sdk_internal_generate_private_headers(HemeraGenerators hemeraqtsdkgeneratorslib_PVTHEADERS)

if (NOT GENERATE_HEADERS_ONLY)
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(Generators STATIC ${hemeraqtsdkgeneratorslib_SRCS})
    target_link_libraries(Generators gcov)
else ()
    add_library(Generators SHARED ${hemeraqtsdkgeneratorslib_SRCS})
endif ()

set_target_properties(Generators PROPERTIES
                                 OUTPUT_NAME HemeraQt5SDKGenerators
                                 SOVERSION ${HEMERA_QT5_SDK_ABI_VERSION}
                                 VERSION ${HEMERA_QT5_SDK_VERSION}
                                 PUBLIC_HEADER "${hemeraqtsdkgeneratorslib_HEADERS}")
qt5_use_modules(Generators Core Qml)
target_link_libraries(Generators Core Qml)# ${LIBSYSTEMD_DAEMON_LIBRARIES} ${LIBSYSTEMD_LOGIN_LIBRARIES})

install(TARGETS Generators
        EXPORT  HemeraQt5SDKGeneratorsTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/HemeraGenerators"
        COMPONENT HemeraQt5SDK)

# Export Generators target
install(EXPORT HemeraQt5SDKGeneratorsTargets NAMESPACE HemeraQt5SDK:: DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKGenerators)

set(GENERATORS_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK" "\${CMAKE_CURRENT_LIST_DIR}/${REL_INCLUDE_DIR}/HemeraQt5SDK/HemeraGenerators")
configure_file(HemeraQt5SDKGeneratorsConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGeneratorsConfig.cmake @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGeneratorsConfigVersion.cmake @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGeneratorsConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGeneratorsConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDKGenerators COMPONENT HemeraQt5SDK)

# Pkgconfig
configure_file(HemeraQt5SDKGenerators.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGenerators.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HemeraQt5SDKGenerators.pc
        DESTINATION ${INSTALL_PKGCONFIG_DIR} COMPONENT HemeraQt5SDK)

# Install private headers, if requested
if (HEMERA_QT5_SDK_PRIVATE_COMPONENTS)
    install(FILES ${hemeraqtsdkgeneratorslib_PVTHEADERS}
            DESTINATION "${INSTALL_INCLUDE_DIR}/HemeraQt5SDK/private/HemeraGenerators"
            COMPONENT private)
endif ()
endif ()
