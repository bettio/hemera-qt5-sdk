/*
 *
 */

#ifndef HEMERA_GENERATORS_GENERATORPLUGINMANAGER_H
#define HEMERA_GENERATORS_GENERATORPLUGINMANAGER_H

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QPluginLoader>

#include <iostream>

class QQmlEngine;
namespace Hemera {
namespace Generators {

template < class T, class P >
class HEMERA_QT5_SDK_EXPORT GeneratorPluginManager
{
public:
    explicit GeneratorPluginManager(const QString &pluginsDirPath, P *parent = nullptr) {
        // Load all the plugins
        QDir pluginsDir(pluginsDirPath);

        for (const QString &fileName : pluginsDir.entryList(QStringList() << QStringLiteral("*.so"), QDir::Files)) {
            // Create plugin loader
            QPluginLoader* pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName), parent);
            // Load plugin
            if (!pluginLoader->load()) {
                pluginLoader->deleteLater();
                std::cout << "-- WARNING: Plugin " << fileName.toLatin1().constData() << " could not be loaded. Reported error: "
                            << pluginLoader->errorString().toLatin1().constData() << std::endl;
                std::cout << "-- \tBecause of the previous warning, your target might potentially fail configuration. Please check your SDK installation."
                            << std::endl;
                continue;
            }

            // Create plugin instance
            QObject *plugin = pluginLoader->instance();
            if (plugin) {
                // Plugin instance created
                T *generatorPlugin  = qobject_cast<T*>(plugin);
                if (!generatorPlugin) {
                    // Interface was wrong
                    pluginLoader->deleteLater();
                    std::cout << "-- WARNING: Plugin " << fileName.toLatin1().constData() << " could not be loaded. "
                                << "There seems to be a mismatch in its symbols, your SDK installation is likely corrupted." << std::endl;
                    std::cout << "-- \tBecause of the previous warning, your target might potentially fail configuration. Please check your SDK installation."
                                << std::endl;
                    continue;
                }

                generatorPlugin->d->baseGenerator = parent;
                m_plugins.append(generatorPlugin);
            }

            pluginLoader->deleteLater();
        }
    }
    virtual ~GeneratorPluginManager() {}

protected:
    inline bool isError() const { return m_plugins.isEmpty(); }

    inline QList< T* > plugins() const { return m_plugins; }

private:
    QList< T* > m_plugins;
};

}
}

#endif // HEMERA_GENERATORS_GENERATORPLUGINMANAGER_H
