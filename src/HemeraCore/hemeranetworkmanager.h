#ifndef HEMERA_NETWORKMANAGER_H
#define HEMERA_NETWORKMANAGER_H

#include <HemeraCore/AsyncInitObject>

namespace Hemera {

class NetworkManagerPrivate;
class NetworkManager : public Hemera::AsyncInitObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_h_ptr, NetworkManager)

public:
    explicit NetworkManager();
    virtual ~NetworkManager();

    bool offlineModeEnabled() const;

    

protected:
    virtual void initImpl() override final;
};
}

#endif // HEMERA_NETWORKMANAGER_H
