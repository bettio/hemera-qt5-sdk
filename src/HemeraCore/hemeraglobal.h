#ifndef HEMERA_GLOBAL_H
#define HEMERA_GLOBAL_H

#include <QtCore/QtGlobal>

/** @defgroup GlobalMacros Global macros for Hemera */

#ifdef BUILDING_HEMERA_QT5_SDK
#  define HEMERA_QT5_SDK_EXPORT Q_DECL_EXPORT
#else
#  define HEMERA_QT5_SDK_EXPORT Q_DECL_IMPORT
#endif

#if !defined(Q_OS_WIN) && defined(QT_VISIBILITY_AVAILABLE)
#  define HEMERA_QT5_SDK_NO_EXPORT __attribute__((visibility("hidden")))
#endif

#ifndef HEMERA_QT5_SDK_NO_EXPORT
#  define HEMERA_QT5_SDK_NO_EXPORT
#endif

/**
 * @def HEMERA_QT5_SDK_DEPRECATED
 * @ingroup GlobalMacros
 *
 * The HEMERA_QT5_SDK_DEPRECATED macro can be used to trigger compile-time
 * warnings with newer compilers when deprecated functions are used.
 *
 * For non-inline functions, the macro gets inserted at front of the
 * function declaration, right before the return type:
 *
 * \code
 * HEMERA_QT5_SDK_DEPRECATED void deprecatedFunctionA();
 * HEMERA_QT5_SDK_DEPRECATED int deprecatedFunctionB() const;
 * \endcode
 *
 * For functions which are implemented inline,
 * the HEMERA_QT5_SDK_DEPRECATED macro is inserted at the front, right before the
 * return type, but after "static", "inline" or "virtual":
 *
 * \code
 * HEMERA_QT5_SDK_DEPRECATED void deprecatedInlineFunctionA() { .. }
 * virtual HEMERA_QT5_SDK_DEPRECATED int deprecatedInlineFunctionB() { .. }
 * static HEMERA_QT5_SDK_DEPRECATED bool deprecatedInlineFunctionC() { .. }
 * inline HEMERA_QT5_SDK_DEPRECATED bool deprecatedInlineFunctionD() { .. }
 * \endcode
 *
 * You can also mark whole structs or classes as deprecated, by inserting the
 * HEMERA_QT5_SDK_DEPRECATED macro after the struct/class keyword, but before the
 * name of the struct/class:
 *
 * \code
 * class HEMERA_QT5_SDK_DEPRECATED DeprecatedClass { };
 * struct HEMERA_QT5_SDK_DEPRECATED DeprecatedStruct { };
 * \endcode
 *
 * \note If the class you want to deprecate is a QObject and needs to be exported,
 *       you should use HEMERA_QT5_SDK_EXPORT_DEPRECATED instead.
 *
 * \note
 * It does not make much sense to use the HEMERA_QT5_SDK_DEPRECATED keyword for a
 * Qt signal; this is because usually get called by the class which they belong
 * to, and one would assume that a class author does not use deprecated methods
 * of his own class. The only exception to this are signals which are connected
 * to other signals; they get invoked from moc-generated code. In any case,
 * printing a warning message in either case is not useful.
 * For slots, it can make sense (since slots can be invoked directly) but be
 * aware that if the slots get triggered by a signal, they will get called from
 * moc code as well and thus the warnings are useless.
 *
 * \note
 * HEMERA_QT5_SDK_DEPRECATED cannot be used for constructors.
 */
#ifndef HEMERA_QT5_SDK_DEPRECATED
#  ifdef HEMERA_QT5_SDK_DEPRECATED_WARNINGS
#    ifdef BUILDING_HEMERA_QT5_SDK
#      define HEMERA_QT5_SDK_DEPRECATED
#    else
#      define HEMERA_QT5_SDK_DEPRECATED Q_DECL_DEPRECATED
#    endif
#  else
#    define HEMERA_QT5_SDK_DEPRECATED
#  endif
#endif

/**
 * @def HEMERA_QT5_SDK_EXPORT_DEPRECATED
 * @ingroup GlobalMacros
 *
 * The HEMERA_QT5_SDK_EXPORT_DEPRECATED macro can be used to trigger compile-time
 * warnings with newer compilers when deprecated functions are used, and
 * export the symbol.
 *
 * This macro simply expands to HEMERA_QT5_SDK_DEPRECATED HEMERA_QT5_SDK_EXPORT, and needs
 * to be used only when you need to deprecate a class which is a QObject
 * and needs to be exported. This is because the following:
 *
 * \code
 * class HEMERA_QT5_SDK_DEPRECATED HEMERA_QT5_SDK_EXPORT Class : public QObject
 * \endcode
 *
 * Wouldn't be recognized from moc to be a valid QObject class, and hence
 * would be skipped. Instead, you should do:
 *
 * \code
 * class HEMERA_QT5_SDK_EXPORT_DEPRECATED Class : public QObject
 * \endcode
 *
 * For any other use, please use HEMERA_QT5_SDK_DEPRECATED instead.
 */
#ifndef HEMERA_QT5_SDK_EXPORT_DEPRECATED
#  define HEMERA_QT5_SDK_EXPORT_DEPRECATED HEMERA_QT5_SDK_DEPRECATED HEMERA_QT5_SDK_EXPORT
#endif

////////////// ENUMS VALUES
/*
 * We're binding the SDK n ways, so each Enum value must be accessible both from Qt5 and C++ side.
 * So, we define here conveniency for some enum values.
 */

#define HEMERA_ENUM_Application_ApplicationStatus_NotInitialized 0
#define HEMERA_ENUM_Application_ApplicationStatus_Initializing 1
#define HEMERA_ENUM_Application_ApplicationStatus_Stopped 2
#define HEMERA_ENUM_Application_ApplicationStatus_Starting 3
#define HEMERA_ENUM_Application_ApplicationStatus_Running 4
#define HEMERA_ENUM_Application_ApplicationStatus_Stopping 5
#define HEMERA_ENUM_Application_ApplicationStatus_Failed 252
#define HEMERA_ENUM_Application_ApplicationStatus_ShuttingDown 253
#define HEMERA_ENUM_Application_ApplicationStatus_ReadyForShutdown 254
#define HEMERA_ENUM_Application_ApplicationStatus_Unknown 255

#define HEMERA_ENUM_Application_ResourceDomain_Defaults 0
#define HEMERA_ENUM_Application_ResourceDomain_Application 1
#define HEMERA_ENUM_Application_ResourceDomain_Orbit 2

#define HEMERA_ENUM_Application_OperationResult_Failed 0
#define HEMERA_ENUM_Application_OperationResult_Success 1
#define HEMERA_ENUM_Application_OperationResult_Fatal 2

#define HEMERA_ENUM_Gravity_OrbitType_Unknown 0
#define HEMERA_ENUM_Gravity_OrbitType_Standalone 1
#define HEMERA_ENUM_Gravity_OrbitType_Planet 2
#define HEMERA_ENUM_Gravity_OrbitType_Satellite 3

#define HEMERA_ENUM_ServiceInfo_ServiceType_Unknown 0
#define HEMERA_ENUM_ServiceInfo_ServiceType_Application 1
#define HEMERA_ENUM_ServiceInfo_ServiceType_Orbit 2
#define HEMERA_ENUM_ServiceInfo_ServiceType_RootOperation 3

#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_None 0
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RefreshRepositories 1
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateApplications 2
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_UpdateSystem 3
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_InstallApplications 4
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationType_RemoveApplications 5

#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_NoStep 0
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Download 1 << 0
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Process 1 << 1
#define HEMERA_ENUM_SoftwareManager_ProgressReporter_OperationStep_Cleanup 1 << 2

#endif
