#ifndef HEMERA_HEMERAHEADLESSAPPLICATION_H
#define HEMERA_HEMERAHEADLESSAPPLICATION_H

#include <HemeraCore/Application>

#include <QtCore/QCoreApplication>

namespace Hemera {

class HeadlessApplicationPrivate;
/**
 * @class HeadlessApplication
 * @ingroup HemeraCore
 * @headerfile HemeraCore/hemeraheadlessapplication.h <HemeraCore/HeadlessApplication>
 *
 * @brief The base class for Hemera Headless applications
 *
 * HeadlessApplication holds all the needed logic for applications with no User Interface.
 *
 * It follows all of the paradigms of Application, and does not add any other layer to Application's
 * methods. Subclass HeadlessApplication when your application does not need to display anything
 * on the screen.
 */
class HEMERA_QT5_SDK_EXPORT HeadlessApplication : public Hemera::Application
{
    Q_OBJECT
    Q_DISABLE_COPY(HeadlessApplication)
    Q_DECLARE_PRIVATE_D(d_h_ptr, HeadlessApplication)

public:
    /// Default constructor
    explicit HeadlessApplication(ApplicationProperties *properties);
    /// Default destructor
    virtual ~HeadlessApplication();
};

}

#endif // HEMERA_HEMERAHEADLESSAPPLICATION_H
