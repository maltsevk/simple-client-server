#include "client_udp.h"

#include <iostream>
#include <thread>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MESSAGE_MAX_SIZE 1024

ClientUDP::ClientUDP(const std::string& address, int port) :
    Client(address, port)
{
}

ClientUDP::~ClientUDP()
{
    ::close(m_socket);
}

bool ClientUDP::initialize()
{
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0)
    {
        std::cout << "[-] socket() failed: " << m_socket << std::endl;
        return false;
    }

    return true;
}

// TODO: implement UDP interaction using confirmation messages
// maybe there is a need to use json format

bool ClientUDP::send(const std::string& message) const
{
    int message_len = message.length() + 1; // include null terminator
    int total_sent = 0;

    while (total_sent < message_len)
    {
        int bytes_sent = ::sendto(m_socket, message.c_str() + total_sent, message_len - total_sent, 
                                  0, (struct sockaddr *)&m_address, sizeof(m_address));
        if (bytes_sent < 0)
        {
            std::cout << "[-] sendto() failed " << std::endl;
            return false;
        }

        total_sent += bytes_sent;
    }

    return true;
}

bool ClientUDP::receive(std::string& message) const
{
    char buffer[MESSAGE_MAX_SIZE] = { 0 };
    int total_recv = 0;

    while (true) 
    {
        // TODO: check that the received message exactly from the server (by address)

        int bytes_recv = ::recvfrom(m_socket, buffer + total_recv, sizeof(buffer) - total_recv, 
                                    0, nullptr, nullptr);
        if (bytes_recv < 0)
        {
            std::cout << "[-] recv() failed " << std::endl;
            return false;
        }
        else if (bytes_recv == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        total_recv += bytes_recv;

        // if the last received byte is null terminator then 
        // it means the end of the transmission
        if (buffer[total_recv - 1] == '\0')
            break;
    }

    message = buffer;

    return true;
}
