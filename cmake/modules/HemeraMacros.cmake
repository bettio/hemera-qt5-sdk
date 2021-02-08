function(hemera_application_components_install _target_name _application_id _ha_file)
    get_filename_component(_infile ${_ha_file} ABSOLUTE)
    if (NOT EXISTS ${_infile})
        message(FATAL_ERROR "Cannot find file ${_infile}. Aborting.")
    endif ()
    if (NOT ${_application_id} MATCHES "^[a-z0-9-]+(\\.[a-z0-9-]+)+")
        message(FATAL_ERROR "The application id's syntax is wrong. The application id should look like 'com.organization.appname'. Aborting.")
    endif ()

    install(FILES ${_infile}
            DESTINATION ${HEMERA_SERVICE_DIR})

    # Easy stuff - done. Now for real action.
    ### Static file generation
    set(GENFILEARGS
        ${_infile}
        ${_target_name})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.service
               ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}_debug.service
               ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.hemeraservice
        COMMAND ${HA_TOOLS_DIR}/hemera-application-gen
        ARGS ${GENFILEARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${HA_TOOLS_DIR}/hemera-application-gen
        COMMENT "Generating support files for ${_application_id}..." VERBATIM)
    add_custom_target(gen-${_application_id} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.service
                                                         ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.hemeraservice)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.service
            DESTINATION ${HA_SYSTEMD_USER_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}_debug.service
            DESTINATION ${HA_SYSTEMD_USER_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_application_id}.hemeraservice
            DESTINATION ${HEMERA_SERVICE_DIR})

    ### Spec file generation
    set(GENFILEARGS
        ${_infile}
        "cmake"
        ${CMAKE_BINARY_DIR}/install_manifest.txt)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target_name}.spec
        COMMAND ${HA_TOOLS_DIR}/hemera-spec-gen
        ARGS ${GENFILEARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${HA_TOOLS_DIR}/hemera-spec-gen
        COMMENT "Generating spec file for ${_application_id}..." VERBATIM)
    add_custom_target(generate-spec-file DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_target_name}.spec)

    # Or local
    set(GENFILEARGS
        ${_infile}
        "cmake-local"
        ${CMAKE_BINARY_DIR}/install_manifest.txt)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target_name}-local.spec
        COMMAND ${HA_TOOLS_DIR}/hemera-spec-gen
        ARGS ${GENFILEARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${HA_TOOLS_DIR}/hemera-spec-gen
        COMMENT "Generating local spec file for ${_application_id}..." VERBATIM)
    add_custom_target(generate-spec-file-local DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_target_name}-local.spec)
endfunction()

function(hemera_application_static_install _target_name _application_id _ha_file)
    hemera_application_components_install(${_target_name} ${_application_id} ${_ha_file})
endfunction()

function(hemera_application_binary_install _target_name _application_id _ha_file)
    hemera_application_components_install(${_target_name} ${_application_id} ${_ha_file})

    # Install the binary target
    install(TARGETS ${_target_name}
            RUNTIME DESTINATION "${HA_INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${HA_INSTALL_LIB_DIR}" COMPONENT shlib)

    # Additional needed dependencies
    add_dependencies(${_target_name} gen-${_application_id})
endfunction(hemera_application_binary_install)

function(hemera_application_add_qt5_properties _sources _ha_file _class_name)
    get_filename_component(_infile ${_ha_file} ABSOLUTE)
    if (NOT EXISTS ${_infile})
        message(FATAL_ERROR "Cannot find file ${_infile}. Aborting.")
    endif ()

    if (ARGV3)
        set(_basename ${ARGV3} )
    else ()
        string(REGEX REPLACE "(.*[/\\.])?([^\\.]+)\\.ha" "\\2properties" _basename ${_infile})
        string(TOLOWER ${_basename} _basename)
    endif ()

    set(_header "${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h")
    set(_impl   "${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cpp")

    set(GENDATAARGS
        "--sdk=qt5"
        "--output-name=${CMAKE_CURRENT_BINARY_DIR}/${_basename}"
        "--class-name=${_class_name}"
        ${_infile})
    add_custom_command(OUTPUT ${_header} ${_impl}
        COMMAND ${HA_TOOLS_DIR}/hemera-properties-gen
        ARGS ${GENDATAARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${HA_TOOLS_DIR}/hemera-properties-gen
        COMMENT "Generating ${_basename}.h, ${_basename}.cpp..." VERBATIM)

    list(APPEND ${_sources} "${_impl}" "${_header}")
    set(${_sources} ${${_sources}} PARENT_SCOPE)

endfunction(hemera_application_add_qt5_properties)

function(hemera_application_add_python_properties _ha_file _install_path _class_name)
    get_filename_component(_infile ${_ha_file} ABSOLUTE)
    if (NOT EXISTS ${_infile})
        message(FATAL_ERROR "Cannot find file ${_infile}. Aborting.")
    endif ()

    if (ARGV3)
        set(_basename ${ARGV3} )
    else ()
        string(REGEX REPLACE "(.*[/\\.])?([^\\.]+)\\.ha" "\\2properties" _basename ${_infile})
        string(TOLOWER ${_basename} _basename)
    endif ()

    set(_pyfile "${CMAKE_CURRENT_BINARY_DIR}/${_basename}.py")

    set(GENDATAARGS
        "--sdk=python"
        "--output-name=${CMAKE_CURRENT_BINARY_DIR}/${_basename}"
        "--class-name=${_class_name}"
        ${_infile})
    add_custom_command(OUTPUT ${_pyfile}
        COMMAND ${HA_TOOLS_DIR}/hemera-properties-gen
        ARGS ${GENDATAARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${HA_TOOLS_DIR}/hemera-properties-gen
        COMMENT "Generating ${_basename}.py..." VERBATIM)

    add_custom_target(gen-python-properties-${_class_name} ALL DEPENDS ${_pyfile})
    install(FILES ${_pyfile} DESTINATION ${_install_path})

endfunction(hemera_application_add_python_properties)

function(hemera_service_add_icon _application_id _icon_file)
    get_filename_component(_icon_extension ${_icon_file} EXT)
    install(FILES ${_icon_file} DESTINATION ${HS_ICONS_DIR} RENAME ${_application_id}${_icon_extension})
endfunction(hemera_service_add_icon)

macro(hemera_setup_test_targets _PROJECT_NAME _ENABLE_COVERAGE) #_COVERAGE_THRESHOLD
    set(_COVERAGE_THRESHOLD "${ARGV2}")

    enable_testing()

    find_program(SH sh)
    # Add targets for callgrind and valgrind tests
    add_custom_target(check-valgrind)
    add_custom_target(check-callgrind)
    add_custom_target(check)
    # We have to deal with the fact that there's no such thing as the "test" target...
    add_custom_target(perform-tests make test || true
                      COMMENT "Performing tests for coverage..."
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

    if (${_ENABLE_COVERAGE})
        # Check dependencies and the likes
        check_cxx_accepts_flag("--coverage" CXX_COVERAGE_SUPPORT)

        if (CXX_COVERAGE_SUPPORT)
            find_program(LCOV lcov)
            find_program(LCOV_GENHTML genhtml)
            find_program(GCOVR gcovr)
            if ((NOT LCOV OR NOT LCOV_GENHTML) AND NOT GCOVR)
                message(FATAL_ERROR "You chose to use compiler coverage tests, but lcov, gcovr or genhtml could not be found in your PATH.")
            else ()
                message(STATUS "Compiler coverage tests enabled - ${_PROJECT_NAME} will be compiled as a static library")
                add_definitions("--coverage")
            endif ()
        else (CXX_COVERAGE_SUPPORT)
            message(FATAL_ERROR "You chose to use compiler coverage tests, but it appears your compiler is not capable of supporting them. Aborting.")
        endif (CXX_COVERAGE_SUPPORT)

        add_custom_target(gcda-reset find ${CMAKE_BINARY_DIR} -name '*.gcda' -exec rm -f '{}' '\;' || true
                          COMMENT "Cleaning gcda (coverage) files")
        add_dependencies(perform-tests gcda-reset)

        if (LCOV)
            message(STATUS "Adding support for lcov - HTML reports will be built.")

            # Add targets for lcov reports
            add_custom_target(lcov-reset ${LCOV} --directory ${CMAKE_BINARY_DIR} --zerocounters COMMENT "Cleaning lcov files")
            add_dependencies(gcda-reset lcov-reset)
            add_custom_target(lcov-check ${LCOV} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${CMAKE_BINARY_DIR}/lcov.info &&
                                        mkdir ${CMAKE_BINARY_DIR}/lcov.html || true && genhtml --title "${_PROJECT_NAME}"
                                        --output-directory ${CMAKE_BINARY_DIR}/lcov.html ${CMAKE_BINARY_DIR}/lcov.info
                              COMMENT "Generating lcov report in file://${CMAKE_BINARY_DIR}/lcov.html/index.html"
                              WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
            add_dependencies(lcov-check perform-tests)

            add_dependencies(check lcov-check)
        endif ()

        if (GCOVR)
            MESSAGE(STATUS "Enabling XML coverage report - coverage threshold checks will be enabled.")
            SET(COVERAGE_XML_FILE "${CMAKE_BINARY_DIR}/coverage.xml")

            add_custom_target(coverage-xml ${GCOVR} -r ${CMAKE_BINARY_DIR} -x -o ${COVERAGE_XML_FILE}
                              COMMENT "Generating coverage XML report to ${COVERAGE_XML_FILE}"
                              WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

            add_dependencies(coverage-xml perform-tests)
            add_dependencies(check coverage-xml)

            # Check coverage target
            if (_COVERAGE_THRESHOLD)
                add_custom_target(check-coverage-threshold ${HEMERA_COVERAGE_TEST_PROGRAM} ${COVERAGE_XML_FILE} ${_COVERAGE_THRESHOLD}
                                  COMMENT "Checking coverage is above ${_COVERAGE_THRESHOLD}%...")
                add_dependencies(check-coverage-threshold coverage-xml)
            endif ()
        endif ()
    endif ()
endmacro()

function(hemera_setup_dbus_test_environment)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/runDbusTest.sh "
${test_environment}
RUNNING_AUTOTESTS=1 sh ${CMAKE_SOURCE_DIR}/tools/with-system-bus.sh \\
    --config-file=\"${CMAKE_BINARY_DIR}/tests/dbus-1/system.conf\" -- \\
    sh ${CMAKE_SOURCE_DIR}/tools/with-session-bus.sh \\
    --config-file=\"${CMAKE_BINARY_DIR}/tests/dbus-1/session.conf\" -- $@
")
endfunction(hemera_setup_dbus_test_environment)

# add the -i option to QT_GENERATE_MOC
function(hemera_generate_moc_i infile outfile)
    qt5_get_moc_flags(moc_flags)
    get_filename_component(abs_infile ${infile} ABSOLUTE)
    qt5_create_moc_command(${abs_infile} ${outfile} "${moc_flags}" "-i" "" "")
    set_source_files_properties(${infile} ${outfile} PROPERTIES SKIP_AUTOMOC TRUE)  # dont run automoc on this file
endfunction(hemera_generate_moc_i)

macro(hemera_add_dbus_unit_test _fancyName _name) # _libraries
    hemera_generate_moc_i(${_name}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp.moc.hpp)
    add_executable(test-${_name} ${_name}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp.moc.hpp)
    if (IN_HEMERA_QT5_SDK_TREE)
        target_link_libraries(test-${_name} Core Test)
    else ()
        target_link_libraries(test-${_name} HemeraQt5SDK::Core HemeraQt5SDK::Test)
    endif ()
    target_link_libraries(test-${_name} Qt5::Core Qt5::Network Qt5::DBus Qt5::Test ${ARGN})
    set(with_system_bus ${CMAKE_CURRENT_BINARY_DIR}/runDbusTest.sh)
    set(_test_script ${SH} ${with_system_bus} ${CMAKE_CURRENT_BINARY_DIR}/test-${_name})
    add_test(${_fancyName} ${_test_script})

    # Valgrind and Callgrind targets
    _hemera_add_check_targets(${_fancyName} ${_name} ${_test_script})
endmacro(hemera_add_dbus_unit_test _fancyName _name)

macro(hemera_add_unit_test _fancyName _name) # _libraries
    hemera_generate_moc_i(${_name}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp.moc.hpp)
    add_executable(test-${_name} ${_name}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp.moc.hpp)
    if (IN_HEMERA_QT5_SDK_TREE)
        target_link_libraries(test-${_name} Core Test)
    else ()
        target_link_libraries(test-${_name} HemeraQt5SDK::Core HemeraQt5SDK::Test)
    endif()
    target_link_libraries(test-${_name} Qt5::Core Qt5::Network Qt5::Test ${ARGN})
    add_test(${_fancyName} ${CMAKE_CURRENT_BINARY_DIR}/test-${_name})

    # Valgrind and Callgrind targets
    _hemera_add_check_targets(${_fancyName} ${_name} ${CMAKE_CURRENT_BINARY_DIR}/test-${_name})
endmacro()

macro(_hemera_add_check_targets _fancyName _name)
    set_tests_properties(${_fancyName}
        PROPERTIES
            FAIL_REGULAR_EXPRESSION "^FAIL!")

    # Standard check target
    add_custom_target(check-${_fancyName} ${ARGN})
    add_dependencies(check-${_fancyName} test-${_name})

    # Coverage targets
    add_dependencies(perform-tests test-${_name})

    # Valgrind target
    add_custom_target(check-valgrind-${_fancyName})
    add_dependencies(check-valgrind-${_fancyName} test-${_name})

    add_custom_command(
        TARGET  check-valgrind-${_fancyName}
        COMMAND G_SLICE=always-malloc ${SH} ${_runnerScript} /usr/bin/valgrind
                --tool=memcheck
                --leak-check=full
                --leak-resolution=high
                --child-silent-after-fork=yes
                --num-callers=20
                --gen-suppressions=all
                --log-file=${CMAKE_CURRENT_BINARY_DIR}/test-${_fancyName}.memcheck.log
                --suppressions=${CMAKE_SOURCE_DIR}/tools/hemera-qt-sdk.supp
                --xml=yes
                --xml-file=${CMAKE_CURRENT_BINARY_DIR}/test-${_fancyName}.memcheck.xml
                ${ARGN}
        WORKING_DIRECTORY
                ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running valgrind on test \"${_fancyName}\"")
    add_dependencies(check-valgrind check-valgrind-${_fancyName})

    # Callgrind target
    add_custom_target(check-callgrind-${_fancyName})
    add_dependencies(check-callgrind-${_fancyName} test-${_name})
    add_custom_command(
        TARGET  check-callgrind-${_fancyName}
        COMMAND ${SH} ${_runnerScript} /usr/bin/valgrind
                --tool=callgrind
                --dump-instr=yes
                --log-file=${CMAKE_CURRENT_BINARY_DIR}/test-${_fancyName}.callgrind.log
                --callgrind-out-file=${CMAKE_CURRENT_BINARY_DIR}/test-${_fancyName}.callgrind.out
                ${ARGN}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Running callgrind on test \"${_fancyName}\"")
    add_dependencies(check-callgrind check-callgrind-${_fancyName})
endmacro(_hemera_add_check_targets _fancyName _name)

macro(hemera_qt5_sdk_use_modules _target _link_type)
    if (NOT TARGET ${_target})
        message(FATAL_ERROR "The first argument to hemera_qt5_sdk_use_modules must be an existing target.")
    endif()
    if ("${_link_type}" STREQUAL "LINK_PUBLIC" OR "${_link_type}" STREQUAL "LINK_PRIVATE" )
        set(_hemera_qt5_sdk_modules ${ARGN})
        set(_hemera_qt5_sdk_link_type ${_link_type})
    else()
        set(_hemera_qt5_sdk_modules ${_link_type} ${ARGN})
    endif()

    if ("${_hemera_qt5_sdk_modules}" STREQUAL "")
        message(FATAL_ERROR "hemera_qt5_sdk_use_modules requires at least one Hemera Qt SDK module to use.")
    endif()

    foreach(_module ${_hemera_qt5_sdk_modules})
        if (NOT HemeraQt5SDK${_module}_FOUND)
            find_package(HemeraQt5SDK${_module} PATHS "${_HemeraQt5SDK_COMPONENT_PATH}" NO_DEFAULT_PATH)
            if (NOT HemeraQt5SDK${_module}_FOUND)
                message(FATAL_ERROR "Can not use \"${_module}\" module which has not yet been found.")
            endif()
        endif()
        target_link_libraries(${_target} ${_hemera_qt5_sdk_link_type} ${HemeraQt5SDK${_module}_LIBRARIES})
        set_property(TARGET ${_target} APPEND PROPERTY INCLUDE_DIRECTORIES ${HemeraQt5SDK${_module}_INCLUDE_DIRS})
        set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS ${HemeraQt5SDK${_module}_COMPILE_DEFINITIONS})
        set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)

        if (HemeraQt5SDK_POSITION_INDEPENDENT_CODE)
            set_property(TARGET ${_target} PROPERTY POSITION_INDEPENDENT_CODE ${HemeraQt5SDK_POSITION_INDEPENDENT_CODE})
        endif()
    endforeach()
endmacro()

function(hemera_application_create_package _name _version)
    set(ARCHIVE_NAME ${_name}-${_version})
    string(SUBSTRING ${CMAKE_SOURCE_DIR} 1 -1 STRIPPED_SOURCE_DIR)
    add_custom_target(dist
        COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
            | xz > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.xz
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_custom_target(dist-no-git
    COMMAND tar Jcf ${ARCHIVE_NAME}.tar.xz --transform 's,${STRIPPED_SOURCE_DIR},${ARCHIVE_NAME}/,' --exclude='${CMAKE_BINARY_DIR}/*' ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction(hemera_application_create_package)

function(hemera_rootoperation_binary_install _target_name _operation_id _application_id)
    # We don't need components install: the application macros handles this complexity for us

    # Install systemd files for the root operation (in the system area, as they will be executed out of orbit as root)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-${_operation_id}@.service
            DESTINATION ${HA_SYSTEMD_SYSTEM_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-${_operation_id}.socket
            DESTINATION ${HA_SYSTEMD_SYSTEM_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-${_operation_id}.hemeraservice
            DESTINATION ${HEMERA_SERVICE_DIR})

    # Install the binary target
    install(TARGETS ${_target_name}
            RUNTIME DESTINATION "${HA_ROOTOPERATION_INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${HA_ROOTOPERATION_INSTALL_LIB_DIR}" COMPONENT shlib)

    # Additional needed dependencies: this target requires the application support files to be generated first.
    add_dependencies(${_target_name} gen-${_application_id})
endfunction(hemera_rootoperation_binary_install)
