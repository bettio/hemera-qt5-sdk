#ifndef HEMERA_QML_HEMERAQMLSOFTWAREMANAGEMENTPLUGIN_H
#define HEMERA_QML_HEMERAQMLSOFTWAREMANAGEMENTPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

namespace Hemera {

namespace Qml {

namespace SoftwareManagement
{

class SoftwareManagementPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    virtual void registerTypes(const char *uri) override final;

    virtual void initializeEngine(QQmlEngine *engine, const char *uri) override final;
};

}

}

}

#endif // HEMERA_QML_HEMERAQMLSOFTWAREMANAGEMENTPLUGIN_H
