project(hemera-qt5-sdk CXX)

cmake_minimum_required(VERSION 2.8.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
find_package(CommonModules 3 REQUIRED)

#################################################################################################
# Modify these variables to what suits the application best. Remember, no dashes in the version!
set(HEMERA_QT5_SDK_ABI_VERSION 0)
set(HEMERA_QT5_SDK_MINOR_VERSION 91)
set(HEMERA_QT5_SDK_RELEASE_VERSION 97)
option(HEMERA_QT5_SDK_DEVELOPMENT_RELEASE "Must be ON unless we're releasing" ON)

# These paths configure where applications and stuff will be installed
set(HEMERA_APPLICATION_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/libexec/hemera/applications CACHE PATH "Installation directory for Hemera Applications")
set(HEMERA_APPLICATION_BASE_RESOURCE_DIR ${CMAKE_INSTALL_PREFIX}/share/hemera/applications CACHE PATH "Path to base resource directory for applications")
set(HEMERA_APPLICATION_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib/hemera/applications CACHE PATH "Library directory for Hemera Applications")

set(HEMERA_ROOTOPERATION_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/libexec/hemera/rootoperations CACHE PATH "Installation directory for Hemera Root Operations")
set(HEMERA_ROOTOPERATION_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib/hemera/rootoperations CACHE PATH "Library directory for Hemera Root Operations")

set(HEMERA_APPLICATION_OWN_RESOURCE_DIR /var/lib/hemera/applications CACHE PATH "Path to base resource directory for applications")
set(HEMERA_SERVICE_DIR ${CMAKE_INSTALL_PREFIX}/share/hemera/services CACHE PATH "Installation directory for service and descriptor files")
set(HEMERA_SERVICE_ICONS_DIR ${CMAKE_INSTALL_PREFIX}/share/hemera/icons CACHE PATH "Installation directory for service icons")
set(HEMERA_APPLICATION_DEBUG_SCRIPTS_DIR /var/lib/hemera/debug-scripts CACHE PATH "Location of Hemera Applications' debug scripts")
set(HEMERA_APPLICATION_DBUS_SYSTEM_POLICY_FILES_DIR /etc/dbus-1/system.d CACHE PATH "Installation directory for system DBus policy files")
set(HEMERA_APPLICATION_DBUS_SESSION_POLICY_FILES_DIR /etc/dbus-1/session.d CACHE PATH "Installation directory for session DBus policy files")
set(HEMERA_APPLICATION_SYSTEMD_SYSTEM_DIR /lib/systemd/system CACHE PATH "Installation directory for systemd system service files")
set(HEMERA_APPLICATION_SYSTEMD_USER_DIR ${CMAKE_INSTALL_PREFIX}/lib/systemd/user CACHE PATH "Installation directory for systemd user service files")
set(HEMERA_APPLICATION_TOOLS_DIR ${CMAKE_INSTALL_PREFIX}/libexec/hemera/tools CACHE PATH "Installation directory for tools")
set(HEMERA_STARS_RUNTIME_DIR /run/hemera/stars CACHE PATH "Location of Hemera Stars runtime directory.")

option(ENABLE_WERROR "Enables WError. Always enable when developing, and disable when releasing." ON)

option(HEMERA_QT5_SDK_PRIVATE_COMPONENTS "Installs additional private headers and components." OFF)

option(GENERATE_HEADERS_ONLY "If building out of Hemera, it merely configures and generates headers. Multiplatform." OFF)

#################################################################################################

set(HEMERA_QT5_SDK_VERSION ${HEMERA_QT5_SDK_ABI_VERSION}.${HEMERA_QT5_SDK_MINOR_VERSION}.${HEMERA_QT5_SDK_RELEASE_VERSION})
if (HEMERA_QT5_SDK_DEVELOPMENT_RELEASE)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    STRING(SUBSTRING ${GIT_SHA1} 0 8 GIT_SHA1_SMALL)
    set(HEMERA_QT5_SDK_VERSION_STRING ${HEMERA_QT5_SDK_ABI_VERSION}.${HEMERA_QT5_SDK_MINOR_VERSION}.${HEMERA_QT5_SDK_RELEASE_VERSION}+git${GIT_SHA1_SMALL})
else ()
    set(HEMERA_QT5_SDK_VERSION_STRING ${HEMERA_QT5_SDK_ABI_VERSION}.${HEMERA_QT5_SDK_MINOR_VERSION}.${HEMERA_QT5_SDK_RELEASE_VERSION})
endif ()

if (NOT GENERATE_HEADERS_ONLY)
    find_package(Systemd REQUIRED)
endif ()

# We need Qt anyway tho.
if (NOT GENERATE_HEADERS_ONLY)
    find_package(Qt5 5.6.0 CONFIG REQUIRED Core Network DBus Gui Widgets Declarative Qml Quick Test LinguistTools)
else ()
    find_package(Qt5 5.6.0 CONFIG REQUIRED Core Network Gui Widgets Declarative Qml Quick Test)
endif()

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# We want explicit literals all over the place, and fast concat
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_USE_QSTRINGBUILDER)
set(IN_HEMERA_QT5_SDK_TREE TRUE)

include_directories(${SYSTEMD_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/src)

if (NOT GENERATE_HEADERS_ONLY)
    include(ParseArguments)
    include(EnableSlocCount)
    include(GenerateCppcheck)
    include(GitArchive)
    include(CompilerWarnings)
    include(TestCXXAcceptsFlag)
endif ()

# Warnings. Enable WError, this component is critical.
if(${ENABLE_WERROR})
    set(RET_W 1)
else(${ENABLE_WERROR})
    set(RET_W 0)
endif(${ENABLE_WERROR})

set(desired
    all
    extra
    sign-compare
    pointer-arith
    format-security
    init-self
    non-virtual-dtor)
set(undesired
    missing-field-initializers
    unused-parameter)

if (NOT GENERATE_HEADERS_ONLY)
    compiler_warnings(CMAKE_CXX_FLAGS_WARNINGS cxx ${RET_W} "${desired}" "${undesired}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_WARNINGS}")
endif ()

# We require C++1y.
add_definitions("-std=c++1y")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_DATA_DIR share/hemera/sdk CACHE PATH "Installation directory for data files of the SDK")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_DBUS_INTERFACES_DIR share/dbus-1/interfaces CACHE PATH "Installation directory for dbus interface files")
set(INSTALL_QML_PLUGINS_DIR ${INSTALL_LIB_DIR}/hemera/imports CACHE PATH "Installation directory for QML plugin files")
set(INSTALL_COMPILER_PLUGINS_DIR ${INSTALL_LIB_DIR}/hemera/plugins/compiler CACHE PATH "Installation directory for Compiler plugin files")
set(INSTALL_CONFIGURE_PLUGINS_DIR ${INSTALL_LIB_DIR}/hemera/plugins/configure CACHE PATH "Installation directory for Configure plugin files")
set(INSTALL_GENERATOR_PLUGINS_DIR ${INSTALL_LIB_DIR}/hemera/plugins/generator CACHE PATH "Installation directory for Generator plugin files")
set(INSTALL_PROPERTIES_PLUGINS_DIR ${INSTALL_LIB_DIR}/hemera/plugins/properties-gen CACHE PATH "Installation directory for Properties Generator plugin files")
set(INSTALL_QMAKE_MKSPECS_DIR share/qt5/mkspecs/features CACHE PATH "Installation directory for mkspecs (QMake) files")
set(INSTALL_PKGCONFIG_DIR ${INSTALL_LIB_DIR}/pkgconfig CACHE PATH "Installation directory for pkgconfig files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
set(INSTALL_TRANSLATIONS_DIR ${CMAKE_INSTALL_PREFIX}/share/hemera/translations/hemera-qt5-sdk CACHE PATH "Installation directory for Qt5SDK translations")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN DATA INCLUDE CMAKE DBUS_INTERFACES QML_PLUGINS COMPILER_PLUGINS CONFIGURE_PLUGINS GENERATOR_PLUGINS PROPERTIES_PLUGINS TRANSLATIONS)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# Enable compilation of tests and examples by default only if we're not in a submodule
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    option(ENABLE_HEMERA_QT5_SDK_EXAMPLES "Enable compilation of examples" ON)
    option(ENABLE_HEMERA_QT5_SDK_TESTS "Enable compilation of tests" ON)
else (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    option(ENABLE_HEMERA_QT5_SDK_EXAMPLES "Enable compilation of examples" OFF)
    option(ENABLE_HEMERA_QT5_SDK_TESTS "Enable compilation of tests" OFF)
endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_HEMERA_QT5_SDK_COVERAGE "Enable compiler coverage" OFF)
option(ENABLE_HEMERA_QT5_SDK_TEST_CODEPATHS
       "Enable specific codepaths needed for autotests. As they pose a potential security threat, disable when building a release build." ON)

if (ENABLE_HEMERA_QT5_SDK_TEST_CODEPATHS)
    add_definitions(-DENABLE_TEST_CODEPATHS)
endif (ENABLE_HEMERA_QT5_SDK_TEST_CODEPATHS)

# Create paths for the HemeraQt5SDKConfig.cmake and HemeraQt5SDKConfigVersion files
file(RELATIVE_PATH REL_DATA_DIR "${INSTALL_CMAKE_DIR}/HemeraQt5SDK" "${INSTALL_DATA_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}/HemeraQt5SDK" "${INSTALL_INCLUDE_DIR}")
file(RELATIVE_PATH REL_DBUS_INTERFACES_DIR "${INSTALL_CMAKE_DIR}/HemeraQt5SDK" "${INSTALL_DBUS_INTERFACES_DIR}")
file(RELATIVE_PATH REL_QML_PLUGINS_DIR "${INSTALL_CMAKE_DIR}/HemeraQt5SDK" "${INSTALL_QML_PLUGINS_DIR}")
# ... for the install tree
set(CONF_DATA_DIR "\${CMAKE_CURRENT_LIST_DIR}/${REL_DATA_DIR}")
set(DBUS_INTERFACES_DIR "\${CMAKE_CURRENT_LIST_DIR}/${REL_DBUS_INTERFACES_DIR}")
get_filename_component(CONF_QML_PLUGINS_DIR "${INSTALL_CMAKE_DIR}/HemeraQt5SDK/${REL_QML_PLUGINS_DIR}" ABSOLUTE)

if (ENABLE_HEMERA_QT5_SDK_EXAMPLES)
    add_subdirectory(examples)
endif (ENABLE_HEMERA_QT5_SDK_EXAMPLES)

if (ENABLE_HEMERA_QT5_SDK_TESTS AND NOT GENERATE_HEADERS_ONLY)
    include(HemeraMacros)
    # Override variable - we're building in tree
    set(HEMERA_COVERAGE_TEST_PROGRAM "${CMAKE_SOURCE_DIR}/tools/verify-coverage-threshold.py")
    hemera_setup_test_targets("Hemera Qt5 SDK" ${ENABLE_HEMERA_QT5_SDK_COVERAGE} 60)
endif ()

set(PROJECT_VERSION_FULL ${HEMERA_QT5_SDK_VERSION_STRING})

include_directories(${CMAKE_BINARY_DIR}/include)

# sources
add_subdirectory(src)
add_subdirectory(cmake/modules)
add_subdirectory(plugins)
add_subdirectory(share)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(translations)

configure_file(HemeraQt5SDKConfig.cmake.in
  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HemeraQt5SDKConfig.cmake" @ONLY)
configure_file(${COMMONMODULES_CMAKE_DIR}/BasicFindPackageVersion.cmake.in
  "${CMAKE_BINARY_DIR}/HemeraQt5SDKConfigVersion.cmake" @ONLY)

# Install the HemeraQt5SDKConfig.cmake and HemeraQt5SDKConfigVersion.cmake
install(FILES
  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/HemeraQt5SDKConfig.cmake"
  "${CMAKE_BINARY_DIR}/HemeraQt5SDKConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}/HemeraQt5SDK" COMPONENT HemeraQt5SDK)

# Archive
if (NOT GENERATE_HEADERS_ONLY)
    add_make_dist_target(hemera-qt5-sdk ${HEMERA_QT5_SDK_VERSION_STRING})
endif ()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_custom_target(generate-headers-package
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include
    COMMAND tar jcf  ${CMAKE_BINARY_DIR}/hemera-qt5-sdk-${HEMERA_QT5_SDK_VERSION_STRING}-headers.tar.bz2 Hemera*)
add_custom_target(generate-headers-package-with-private
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include
    COMMAND tar jcf  ${CMAKE_BINARY_DIR}/hemera-qt5-sdk-${HEMERA_QT5_SDK_VERSION_STRING}-headers-with-private.tar.bz2 Hemera* private/)

# Full dist target, one to rule them all
add_custom_target(dist-all)
add_dependencies(dist-all dist generate-headers-package generate-headers-package-with-private)
