#include "server_tcp.h"

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "parser.h"

#define QUEUE_MAX_SIZE 5
#define MESSAGE_MAX_SIZE 1024

ServerTCP::ServerTCP(int port) :
    m_socket(-1),
    m_started(true)
{
    m_address.sin_family      = AF_INET;
    m_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_address.sin_port        = htons(port);   
}

ServerTCP::~ServerTCP()
{
    ::close(m_socket);
}

bool ServerTCP::initialize()
{
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        std::cout << "[-] socket() failed: " << m_socket << std::endl;
        return false;
    }

    // set non-blocking socket mode
    fcntl(m_socket, F_SETFL, O_NONBLOCK);

    int result = ::bind(m_socket, (struct sockaddr*)&m_address, 
                      sizeof(m_address));
    if (result < 0)
    {
        std::cout << "[-] bind() failed: " << result << std::endl;
        return false;
    }

    result = ::listen(m_socket, QUEUE_MAX_SIZE);
    if (result < 0)
    {
        std::cout << "[-] listen() failed: " << result << std::endl;
        return false;
    }

    return true;
}

void ServerTCP::run()
{
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    std::cout << "[*] Waiting for incoming connections..." << std::endl;

    while (m_started)
    {
        int client_socket = ::accept(m_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket < 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        char client_ip[128];
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    
        std::cout << "[+] Client [" << client_ip << ":"
                  << ntohs(client_addr.sin_port) << "] connected" <<  std::endl;

        auto connection_handler = std::make_unique<std::thread>([this, client_socket]() {
            handleConnection(client_socket);
        });

        m_handlers.push_back(std::move(connection_handler));
    }

    for (int i = 0; i < m_handlers.size(); ++i)
    {
        if (m_handlers[i])
            m_handlers[i]->join();
    }
}

void ServerTCP::stop()
{
    m_started = false;
}

ServerTCP::Status ServerTCP::send(int socket, const std::string& message) const
{
    int message_len = message.length() + 1; // include null terminator
    int total_sent = 0;

    while (total_sent < message_len)
    {
        int bytes_sent = ::send(socket, message.c_str() + total_sent, message_len - total_sent, 0);
        if (bytes_sent < 0)
            return Status::ERROR;

        total_sent += bytes_sent;
    }

    return Status::SUCCESS;
}

ServerTCP::Status ServerTCP::receive(int socket, std::string& message) const
{
    char buffer[MESSAGE_MAX_SIZE] = { 0 };
    int total_recv = 0;

    while (m_started) 
    {
        int bytes_recv = ::recv(socket, buffer + total_recv, sizeof(buffer) - total_recv, MSG_DONTWAIT);
        if (bytes_recv < 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        else if (bytes_recv == 0)
        {
            return Status::CLOSED;
        }

        total_recv += bytes_recv;

        // if the last received byte is null terminator then 
        // it means the end of the transmission
        if (buffer[total_recv - 1] == '\0')
            break;
    }

    message = buffer;

    return Status::SUCCESS;
}

void ServerTCP::handleConnection(int socket)
{
    std::string client_message;

    while (m_started)
    {
        Status status = receive(socket, client_message);
        if (status == Status::ERROR)
        {
            std::cout << "[-] Failed to receive message from client" << std::endl;
            break;
        }
        else if (status == Status::CLOSED)
        {
            std::cout << "[*] Client disconnected" << std::endl;
            break;
        }

        std::cout << "[*] Message from client: [" << client_message << "]" << std::endl;
    
        Parser parser(client_message);

        status = send(socket, parser.handle()); 
        if (status == Status::ERROR)
        {
            std::cout << "[-] Failed to send message to client" << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
