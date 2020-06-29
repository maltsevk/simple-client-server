#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in;

class Client
{
public:
    Client(const std::string& address, int port) :
        m_socket(-1)
    {
    	m_address.sin_family      = AF_INET;
    	m_address.sin_addr.s_addr = inet_addr(address.c_str());
    	m_address.sin_port        = htons(port);
    }

    virtual ~Client() = default;

    virtual bool initialize() = 0;
    virtual bool send(const std::string& message) const = 0;
    virtual bool receive(std::string& message) const = 0;

protected:
    int m_socket;
    sockaddr_in m_address;
};

#endif // CLIENT_h