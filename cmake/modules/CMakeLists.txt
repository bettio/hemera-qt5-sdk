configure_file(HemeraPaths.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/HemeraPaths.cmake" @ONLY)

set(hemeraqtsdklib_CMAKEMODULES
    HemeraMacros.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HemeraPaths.cmake
)

# CMake modules
install(FILES ${hemeraqtsdklib_CMAKEMODULES}
        DESTINATION ${INSTALL_CMAKE_DIR}/HemeraQt5SDK
        COMPONENT cmake)
