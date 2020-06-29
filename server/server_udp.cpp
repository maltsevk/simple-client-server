#include "server_udp.h"

#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "parser.h"

#define QUEUE_MAX_SIZE 5
#define MESSAGE_MAX_SIZE 1024

ServerUDP::ServerUDP(int port) :
    m_socket(-1),
    m_started(true)
{
    m_address.sin_family      = AF_INET;
    m_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_address.sin_port        = htons(port);   
}

ServerUDP::~ServerUDP()
{
    ::close(m_socket);
}

bool ServerUDP::initialize()
{
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0)
    {
        std::cout << "[-] socket() failed: " << m_socket << std::endl;
        return false;
    }

    int result = ::bind(m_socket, (struct sockaddr*)&m_address, 
                        sizeof(m_address));
    if (result < 0)
    {
        std::cout << "[-] bind() failed: " << result << std::endl;
        return false;
    }

    return true;
}

void ServerUDP::run()
{
    std::cout << "[*] Waiting for incoming messages..." << std::endl;

    while (m_started)
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        std::string client_message;

        Status status = receive(&client_addr, &client_len, client_message);
        if (status == Status::EMPTY)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        else if (status == Status::ERROR)
        {
            std::cout << "[-] Failed to receive message from client" << std::endl;
            continue;
        }

        char client_ip[128];
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
    
        std::cout << "[*] Client [" << client_ip << ":" << ntohs(client_addr.sin_port) 
                  << "] sent a new message: [" << client_message << "]" << std::endl;

        Parser parser(client_message);

        status = send(&client_addr, sizeof(client_addr), parser.handle());
        if (status == Status::ERROR)
        {
            std::cout << "[-] Failed to send message to client" << std::endl;
        }
    }
}

void ServerUDP::stop()
{
    m_started = false;
}

ServerUDP::Status ServerUDP::send(sockaddr_in* client_addr, socklen_t client_len, const std::string& message) const
{
    int message_len = message.length() + 1; // include null terminator
    int total_sent = 0;

    while (total_sent < message_len)
    {
        int bytes_sent = ::sendto(m_socket, message.c_str() + total_sent, message_len - total_sent, 
                                  0, (struct sockaddr *)client_addr, client_len);
        if (bytes_sent < 0)
        {
            return Status::ERROR;
        }

        total_sent += bytes_sent;
    }

    return Status::SUCCESS;
}

ServerUDP::Status ServerUDP::receive(sockaddr_in* client_addr, socklen_t* client_len, std::string& message) const
{
    char buffer[MESSAGE_MAX_SIZE] = { 0 };

    int bytes_recv = ::recvfrom(m_socket, buffer, sizeof(buffer), MSG_DONTWAIT, 
                                (struct sockaddr*)client_addr, client_len);
    if (bytes_recv < 0)
    {
        return Status::EMPTY;
    }
    else if (bytes_recv == 0)
    {
        return Status::SUCCESS;
    }

    // if the last received byte is null terminator then 
    // it means the end of the transmission
    if (buffer[bytes_recv - 1] != '\0')
        return Status::ERROR;

    message = buffer;

    return Status::SUCCESS;
}
