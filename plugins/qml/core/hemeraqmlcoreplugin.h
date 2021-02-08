#ifndef HEMERA_QML_HEMERAQMLCOREPLUGIN_H
#define HEMERA_QML_HEMERAQMLCOREPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

namespace Hemera {

namespace Qml {

namespace Core
{

class CorePlugin : public QQmlExtensionPlugin
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

#endif // HEMERA_QML_HEMERAQMLCOREPLUGIN_H
