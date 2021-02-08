set(hemeraqtsdklib_MISCFILES
    dbus_policy.conf.in
    hemera_systemd.service.in
    hemera_debug_script.sh.in
    hemera_root_operation@.service.in
    hemera_root_operation.socket.in
)

# CMake modules
install(FILES ${hemeraqtsdklib_MISCFILES}
        DESTINATION ${INSTALL_DATA_DIR}
        COMPONENT data)

# DBus and friends
set(hemeraqtsdklib_DBUSIFACES
    dbus/com.ispirata.Hemera.ApplianceCrypto.xml
    dbus/com.ispirata.Hemera.Application.xml
    dbus/com.ispirata.Hemera.Application.DBusHolder.xml
    dbus/com.ispirata.Hemera.DBusObject.xml
    dbus/com.ispirata.Hemera.DeviceManagement.xml
    dbus/com.ispirata.Hemera.Fingerprints.xml
    dbus/com.ispirata.Hemera.Parsec.ApplicationHandler.xml
    dbus/com.ispirata.Hemera.Parsec.xml
    dbus/com.ispirata.Hemera.Planet.xml
    dbus/com.ispirata.Hemera.RemovableStorageManager.xml
    dbus/com.ispirata.Hemera.SoftwareManagement.ApplianceManager.xml
    dbus/com.ispirata.Hemera.SoftwareManagement.ApplicationManager.xml
    dbus/com.ispirata.Hemera.SoftwareManagement.ProgressReporter.xml
    dbus/com.ispirata.Hemera.USBGadgetManager.xml
)

# DBus interfaces
install(FILES ${hemeraqtsdklib_DBUSIFACES}
        DESTINATION ${INSTALL_DBUS_INTERFACES_DIR}
        COMPONENT dbus)

# QMake prf
configure_file(hemera.prf.in
  "${CMAKE_CURRENT_BINARY_DIR}/hemera.prf" @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hemera.prf
        DESTINATION ${INSTALL_QMAKE_MKSPECS_DIR}
        COMPONENT qmake)
