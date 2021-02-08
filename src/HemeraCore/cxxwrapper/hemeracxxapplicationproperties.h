#ifndef HEMERACXX_APPLICATIONPROPERTIES_H
#define HEMERACXX_APPLICATIONPROPERTIES_H

#include <string>

namespace Hemera {
class ApplicationProperties;
}

namespace HemeraCxx {

class ApplicationProperties
{
public:
    virtual ~ApplicationProperties();

    std::string id() const;

    std::string name() const;
    std::string version() const;
    std::string organization() const;

    Hemera::ApplicationProperties *qt5Object();

    explicit ApplicationProperties();

    void setId(const std::string &id);

    void setName(const std::string &name);
    void setVersion(const std::string &version);
    void setOrganization(const std::string &organization);

private:
    class Private;
    Private * const d;
};

// Wrappers
typedef ApplicationProperties ApplicationPropertiesWrapper;

}

#endif // HEMERACXX_APPLICATIONPROPERTIES_H
