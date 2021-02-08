#ifndef HEMERA_QMLSOFTWAREMANAGERPROGRESSREPORTER_H
#define HEMERA_QMLSOFTWAREMANAGERPROGRESSREPORTER_H

#include <HemeraSoftwareManagement/ProgressReporter>

#include <QtCore/QDateTime>

namespace Hemera {

namespace Qml {

namespace SoftwareManagement {

class ProgressReporterPrivate;
class ProgressReporter : public Hemera::SoftwareManagement::ProgressReporter
{
    Q_OBJECT
    Q_DISABLE_COPY(ProgressReporter)

public:
    explicit ProgressReporter(QObject* parent = nullptr);
    virtual ~ProgressReporter();

    QString errorName() const;
    QString errorMessage() const;

Q_SIGNALS:
    void error();

private:
    class Private;
    Private * const d;
};

}

}

}

#endif // HEMERA_QMLSOFTWAREMANAGERPROGRESSREPORTER_H
