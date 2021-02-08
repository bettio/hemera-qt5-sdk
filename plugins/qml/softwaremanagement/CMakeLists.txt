########## QML SoftwareManagement Plugin
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_BINARY_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/HemeraCore
                    ${CMAKE_SOURCE_DIR}/src/HemeraSoftwareManagement)

set(hemeraqmlsoftwaremanagement_SRCS
    hemeraqmlsoftwaremanagementappliancemanager.cpp
    hemeraqmlsoftwaremanagementplugin.cpp
    hemeraqmlsoftwaremanagementprogressreporter.cpp
    hemeraqmlsoftwaremanagementsystemupdate.cpp
)

set(hemeraqmlsoftwaremanagement_QMLFILES
    Hemera/SoftwareManagement/qmldir
    Hemera/SoftwareManagement/com.ispirata.Hemera.SoftwareManagement.qmltypes
)

# QML Plugin
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(HemeraQmlSoftwareManagement STATIC ${hemeraqmlsoftwaremanagement_SRCS})
    target_link_libraries(HemeraQmlSoftwareManagement gcov)
else ()
    add_library(HemeraQmlSoftwareManagement SHARED ${hemeraqmlsoftwaremanagement_SRCS})
endif ()

target_link_libraries(HemeraQmlSoftwareManagement Qt5::Core Qt5::Qml Core SoftwareManagement)

# Install plugin
install(TARGETS HemeraQmlSoftwareManagement
        RUNTIME DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/SoftwareManagement" COMPONENT HemeraQt5SDK
        LIBRARY DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/SoftwareManagement" COMPONENT HemeraQt5SDK
        ARCHIVE DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/SoftwareManagement" COMPONENT HemeraQt5SDK
        COMPONENT HemeraQt5SDK)

# Install qmldir files
install(FILES ${hemeraqmlsoftwaremanagement_QMLFILES}
        DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/SoftwareManagement"
        COMPONENT HemeraQt5SDK)
