#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

#include "client.h"

class ClientTCP : public Client
{
public:
    ClientTCP(const std::string& address, int port);
    ~ClientTCP();

    bool initialize() override;
    bool send(const std::string& message) const override;
    bool receive(std::string& message) const override;

private:
    bool m_is_connected;
};

#endif // CLIENT_TCP_H