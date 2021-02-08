#ifndef HEMERACXX_PROXYQT5APPLICATIONPROPERTIES_H
#define HEMERACXX_PROXYQT5APPLICATIONPROPERTIES_H

#include <HemeraCore/ApplicationProperties>

namespace HemeraCxx {

class ApplicationProperties;

class ProxyQt5ApplicationProperties : public Hemera::ApplicationProperties
{
    friend class HemeraCxx::ApplicationProperties;
};

}

#endif // HEMERACXX_PROXYQT5APPLICATIONPROPERTIES_H
