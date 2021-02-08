########## QML Core Plugin
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_BINARY_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/HemeraCore)

set(hemeraqmlcore_SRCS
    hemeraqmlcoreplugin.cpp
)

set(hemeraqmlcore_QMLFILES
    Hemera/Core/qmldir
    Hemera/Core/com.ispirata.Hemera.Core.qmltypes
)

# QML Plugin
if (ENABLE_HEMERA_QT5_SDK_COVERAGE)
    add_library(HemeraQmlCore STATIC ${hemeraqmlcore_SRCS})
    target_link_libraries(HemeraQmlCore gcov)
else ()
    add_library(HemeraQmlCore SHARED ${hemeraqmlcore_SRCS})
endif ()

target_link_libraries(HemeraQmlCore Qt5::Core Qt5::Qml Core)

# Install plugin
install(TARGETS HemeraQmlCore
        RUNTIME DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/Core" COMPONENT HemeraQt5SDK
        LIBRARY DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/Core" COMPONENT HemeraQt5SDK
        ARCHIVE DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/Core" COMPONENT HemeraQt5SDK
        COMPONENT HemeraQt5SDK)

# Install qmldir files
install(FILES ${hemeraqmlcore_QMLFILES}
        DESTINATION "${INSTALL_QML_PLUGINS_DIR}/com/ispirata/Hemera/Core"
        COMPONENT HemeraQt5SDK)
