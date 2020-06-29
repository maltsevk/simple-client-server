#ifndef SERVER_UDP_H
#define SERVER_UDP_H

#include <string>
#include <vector>
#include <atomic>

#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in;

class ServerUDP
{
public:
    ServerUDP(int port);
    ~ServerUDP();

    bool initialize();
    void run();
    void stop();

private:
    enum class Status
    {
        SUCCESS = 0,
        ERROR   = 1,
        EMPTY   = 2
    };

	Status send(sockaddr_in* client_addr, socklen_t client_len, const std::string& message) const;
	Status receive(sockaddr_in* client_addr, socklen_t* client_len, std::string& message) const;

    int m_socket;
    sockaddr_in m_address;
    std::atomic_bool m_started;
};

#endif // SERVER_UDP_H