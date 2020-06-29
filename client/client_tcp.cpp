#include "client_tcp.h"

#include <iostream>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MESSAGE_MAX_SIZE 1024

ClientTCP::ClientTCP(const std::string& address, int port) :
    Client(address, port)
{
}

ClientTCP::~ClientTCP()
{
    ::close(m_socket);
}

bool ClientTCP::initialize()
{
    m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
    {
        std::cout << "[-] socket() failed: " << m_socket << std::endl;
        return false;
    }

    int result = ::connect(m_socket, (struct sockaddr *)&m_address, sizeof(m_address));
    if (result < 0)
    {
        std::cout << "[-] connect() failed: " << result << std::endl;
        return false;
    }

    std::cout << "[+] Connection to the server is established" << std::endl;

    m_is_connected = true;

    return true;
}

bool ClientTCP::send(const std::string& message) const
{
    if (!m_is_connected)
    {
        std::cout << "[-] Connection to the server is not established " << std::endl;
        return false;
    }

    int message_len = message.length() + 1; // include null terminator
    int total_sent = 0;

    while (total_sent < message_len)
    {
        int bytes_sent = ::send(m_socket, message.c_str() + total_sent, message_len - total_sent, 0);
        if (bytes_sent < 0)
        {
            std::cout << "[-] send() failed " << std::endl;
            return false;
        }

        total_sent += bytes_sent;
    }

    return true;
}

bool ClientTCP::receive(std::string& message) const
{
    if (!m_is_connected)
    {
        std::cout << "[-] Connection to the server is not established " << std::endl;
        return false;
    }

    char buffer[MESSAGE_MAX_SIZE] = { 0 };
    int total_recv = 0;

    while (true) 
    {
        int bytes_recv = ::recv(m_socket, buffer + total_recv, sizeof(buffer) - total_recv, 0);
        if (bytes_recv < 0)
        {
            std::cout << "[-] recv() failed " << std::endl;
            return false;
        }
        else if (bytes_recv == 0)
        {
            std::cout << "[-] Connection closed " << std::endl;
            return false;
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
