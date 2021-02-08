include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraCore
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraCore
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraTest
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraTest
                    ${CMAKE_CURRENT_BINARY_DIR}/HemeraGui
                    ${CMAKE_CURRENT_SOURCE_DIR}/HemeraGui
)

add_subdirectory(qml)
