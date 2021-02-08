#ifndef HEMERA_USBGADGETMANAGER_H
#define HEMERA_USBGADGETMANAGER_H

#include <HemeraCore/AsyncInitObject>
#include <HemeraCore/Global>

namespace Hemera {

class Operation;

class USBGadgetManagerPrivate;

/**
 * @class USBGadgetManager
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemerausbgadgetmanager.h <HemeraCore/USBGadgetManager>
 *
 * @brief Provides access to USB Gadget (OTG) support, allowing to control the USB OTG behavior on the system.
 *
 * USBGadgetManager enables an application to control USB Gadgets on the system, aka USB OTG buses. It allows
 * to specify the mode under which the USB Gadget will operate, if any, and to bring up and down USB support.
 * On selected devices, it also allows to detect hotplugging of the USB cable.
 *
 * Usually, only one single USBGadgetManager object should be instantiated for each application for performance reasons.
 *
 * @par System lock
 *
 * If an application has the right to do so, it can request a system wide lock on the USBGadgetManager. This is useful,
 * for example, for shells, where you want to manage USB gadgets globally, or when in Developer Mode, where Gravity
 * takes hold of the global lock while no application is running besides the default shell.
 *
 * When the System lock is active, no other application besides the application which holds the lock can activate or
 * deactivate a USB Gadget Mode.
 *
 * @par System-wide availability of SoftwareManager
 *
 * USBGadgetManager is part of Gravity, and can be entirely disabled from the vendor. This class serves as a mere interface
 * to Gravity, and its initialization will fail if the system USBGadgetManager is not installed or not available.
 *
 * @sa Application
 */
class HEMERA_QT5_SDK_EXPORT USBGadgetManager : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DISABLE_COPY(USBGadgetManager)
    Q_DECLARE_PRIVATE_D(d_h_ptr, USBGadgetManager)

    Q_PROPERTY(Mode activeMode                READ activeMode                NOTIFY activeModeChanged)
    Q_PROPERTY(Modes availableModes           READ availableModes)
    Q_PROPERTY(bool canDetectCableHotplugging READ canDetectCableHotplugging)
    Q_PROPERTY(USBCableStatus usbCableStatus  READ usbCableStatus            NOTIFY usbCableStatusChanged)
    Q_PROPERTY(bool systemWideLockActive      READ isSystemWideLockActive    NOTIFY systemWideLockChanged)
    Q_PROPERTY(QString systemWideLockOwner    READ systemWideLockOwner       NOTIFY systemWideLockChanged)
    Q_PROPERTY(QString systemWideLockReason   READ systemWideLockReason      NOTIFY systemWideLockChanged)

public:
    /// The various modes in which a USB Gadget can be activated
    enum class Mode {
        /// No mode
        None = 0,
        /// Emulates a Ethernet port, and shares any other connection (e.g.: WiFi) over it.
        EthernetTethering,
        /// Emulates a Ethernet port, and creates a zeroconf P2P bridge to the host.
        EthernetP2P,
        /// Emulates a mass storage.
        MassStorage
    };
    Q_ENUM(Mode)
    Q_DECLARE_FLAGS(Modes, Mode)

    /// Status of the USB Cable
    enum class USBCableStatus {
        /// When device does not support detection of cable hotplugging, this value is returned.
        Unknown = 0,
        /// Cable is unplugged
        Unplugged,
        /// Cable is plugged
        Plugged
    };
    Q_ENUM(USBCableStatus)

    /// Default constructor
    explicit USBGadgetManager(QObject* parent);
    /// Default destructor
    virtual ~USBGadgetManager();

    /**
     * @brief Returns available USB gadget modes on this system.
     *
     * Depending on the hardware and software features installed on the target, this method
     * will return valid Modes you can use to activate USB Gadget.
     *
     * @returns a list of available modes for this system.
     *
     * @sa activate
     */
    Modes availableModes() const;
    /**
     * @returns the currently active mode on this system, if any.
     */
    Mode activeMode() const;

    /**
     * @brief Returns whether this system supports detection of USB cable hotplugging.
     *
     * Hemera is capable of doing a set of heuristics to detect if an USB cable is plugged or not
     * to the OTG port. When the hardware itself is not capable of reporting that correctly (this is most of the times),
     * Hemera tries to find out alternative methods to create a reliable heuristic. If this
     * is possible, this method returns true.
     *
     * @note Most of the times, unless you are perfectly sure your hardware supports this feature or
     *       Hemera support told you so, this method might indeed be unreliable. It is advised to consult
     *       with your manufacturer or local Hemera support before heavily relying at runtime on this method.
     *
     * @returns whether this system supports detection of USB cable hotplugging
     */
    bool canDetectCableHotplugging() const;
    /**
     * @returns the current status of the USB cable
     *
     * @note If canDetectCableHotplugging returns false, this method will always return Unknown.
     *
     * @note Even if canDetectCableHotplugging returns true, this method might return Unknown if heuristics
     *       fail for any reason or are not computable.
     */
    USBCableStatus usbCableStatus() const;

    /**
     * @returns Whether a system wide lock is currently active
     */
    bool isSystemWideLockActive() const;
    /**
     * @returns the application ID of the current system wide lock owner, if any.
     */
    QString systemWideLockOwner() const;
    /**
     * @returns the reason for the current ongoing system wide lock, if any.
     */
    QString systemWideLockReason() const;

    /**
     * @brief Tries to acquire a system wide lock.
     *
     * The application will attempt to acquire a system wide lock on the USB Gadget Manager. This will
     * work only if no other application is holding the lock and your application has the right
     * to request a lock in the system.
     *
     * @p reason The reason for the lock.
     *
     * @returns an Operation representing the ongoing request.
     */
    Operation *acquireSystemWideLock(const QString &reason = QString());
    /**
     * @brief Releases a system wide lock.
     *
     * If the application previously acquired a system wide lock, it will be released upon calling
     * this method. The method will fail if the application does not hold the lock.
     *
     * @note This method cannot be used for forcing a different lock owner to release it.
     *
     * @returns an Operation representing the ongoing request.
     */
    Operation *releaseSystemWideLock();

    /**
     * @brief Requests the activation of the USB Gadget.
     *
     * The application will attempt to activate the USB Gadget through USB Gadget Manager. This will
     * work only if no other application is holding a lock (or if this application holds the lock) and
     * your application has the right to manage USB Gadgets in the system.
     *
     * Depending on the mode you have chosen, you might need to supply additional parameters. These
     * are outlined in the documentation for each mode, and shall be provided in form of a QVariantMap.
     *
     * @p mode The mode the USB Gadget should be activated in.
     * @p arguments A map of parameters, dependent on the chosen mode.
     *
     * @returns an Operation representing the ongoing request.
     */
    Operation *activate(Mode mode, const QVariantMap &arguments = QVariantMap());
    /**
     * @brief Requests the deactivation of the USB Gadget.
     *
     * The application will attempt to deactivate the USB Gadget through USB Gadget Manager. This will
     * work only if no other application is holding a lock (or if this application holds the lock) and
     * your application has the right to manage USB Gadgets in the system.
     *
     * @note When no lock is active, this method might as well deactivate USB Gadget when another application
     *       requested activation. If you do not wish for this to happen, you should acquire the lock first.
     *
     * @p mode The mode the USB Gadget should be activated in.
     * @p arguments A map of parameters, dependent on the chosen mode.
     *
     * @returns an Operation representing the ongoing request.
     */
    Operation *deactivate();

Q_SIGNALS:
    /**
     * @brief Emitted whenever the active mode changes
     *
     * @p mode The new active mode.
     */
    void activeModeChanged(Hemera::USBGadgetManager::Mode mode);
    /**
     * @brief Emitted whenever the system wide lock changes
     *
     * @p active Whether the system wide lock is now active or not.
     */
    void systemWideLockChanged(bool active);

    /**
     * @brief Emitted whenever the USB Cable plug status changes.
     *
     * @p status The new status of the USB cable.
     */
    void usbCableStatusChanged(Hemera::USBGadgetManager::USBCableStatus status);

protected:
    virtual void initImpl() Q_DECL_OVERRIDE Q_DECL_FINAL;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Hemera::USBGadgetManager::Modes)

#endif // HEMERA_USBGADGETMANAGER_H
