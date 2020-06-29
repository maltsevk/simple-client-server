#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

#include "client.h"

class ClientUDP : public Client
{
public:
    ClientUDP(const std::string& address, int port);
    ~ClientUDP();

    bool initialize() override;
    bool send(const std::string& message) const override;
    bool receive(std::string& message) const override;
};

#endif // CLIENT_UDP_H