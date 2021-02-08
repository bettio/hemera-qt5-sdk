#ifndef HEMERA_CXX_GLOBAL
#define HEMERA_CXX_GLOBAL

#include <QtCore/QStringList>

#include <string>
#include <vector>

namespace HemeraCxx {

void __hemera_init(int argc, char **argv);
void __hemera_init(std::vector< std::string > argv);

void __hemera_destroy();

inline static std::vector< std::string > qStringListToVector(const QStringList &list) {
    std::vector< std::string > vector;
    vector.reserve(list.size());

    for (const QString &element : list) {
        vector.push_back(element.toStdString());
    }

    return vector;
}

inline static QStringList vectorToQStringList(const std::vector< std::string > &vector) {
    QStringList list;
    list.reserve(vector.size());

    for (const std::string &element : vector) {
        list.append(QString::fromStdString(element));
    }

    return list;
}

}

#endif
