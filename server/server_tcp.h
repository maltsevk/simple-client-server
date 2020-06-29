#ifndef SERVER_TCP_H
#define SERVER_TCP_H

#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>

#include <netinet/in.h>

typedef struct sockaddr_in sockaddr_in;

class ServerTCP
{
public:
    ServerTCP(int port);
    ~ServerTCP();

    bool initialize();
    void run();
    void stop();

private:
    enum class Status
    {
        SUCCESS = 0,
        ERROR   = 1,
        CLOSED  = 2
    };

    Status send(int socket, const std::string& message) const;
    Status receive(int socket, std::string& message) const;
    void handleConnection(int socket);

    int m_socket;
    sockaddr_in m_address;
    std::vector<std::unique_ptr<std::thread>> m_handlers;
    std::atomic_bool m_started;
};

#endif // SERVER_TCP_H
