# FetchSystemConfigOperation
set(com.ispirata.Hemera.FetchSystemConfigOperation_SRCS
    fetchsystemconfigoperation.cpp
)

add_executable(hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation ${com.ispirata.Hemera.FetchSystemConfigOperation_SRCS})
target_link_libraries(hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation Qt5::Core Core)

configure_file(hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation@.service.in
  "${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation@.service" @ONLY)

# Install phase. We can't use generators here, so live with it.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation@.service
        DESTINATION ${HEMERA_APPLICATION_SYSTEMD_SYSTEM_DIR})
install(FILES hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation.socket
        DESTINATION ${HEMERA_APPLICATION_SYSTEMD_SYSTEM_DIR})
install(FILES hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation.hemeraservice
        DESTINATION ${HEMERA_SERVICE_DIR})

install(TARGETS hemera-rootoperation-com.ispirata.Hemera.FetchSystemConfigOperation
        RUNTIME DESTINATION "${HEMERA_ROOTOPERATION_INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_ROOTOPERATION_INSTALL_LIB_DIR}" COMPONENT shlib
        COMPONENT RootOperations)


# SetSystemConfigOperation
set(com.ispirata.Hemera.SetSystemConfigOperation_SRCS
    setsystemconfigoperation.cpp
)

add_executable(hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation ${com.ispirata.Hemera.SetSystemConfigOperation_SRCS})
target_link_libraries(hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation Qt5::Core Core)

configure_file(hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation@.service.in
  "${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation@.service" @ONLY)

# Install phase. We can't use generators here, so live with it.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation@.service
        DESTINATION ${HEMERA_APPLICATION_SYSTEMD_SYSTEM_DIR})
install(FILES hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation.socket
        DESTINATION ${HEMERA_APPLICATION_SYSTEMD_SYSTEM_DIR})
install(FILES hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation.hemeraservice
        DESTINATION ${HEMERA_SERVICE_DIR})

install(TARGETS hemera-rootoperation-com.ispirata.Hemera.SetSystemConfigOperation
        RUNTIME DESTINATION "${HEMERA_ROOTOPERATION_INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${HEMERA_ROOTOPERATION_INSTALL_LIB_DIR}" COMPONENT shlib
        COMPONENT RootOperations)
