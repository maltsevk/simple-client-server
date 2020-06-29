#include <iostream>
#include <memory>

#include "client_tcp.h"
#include "client_udp.h" 

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT_TCP 7777
#define DEFAULT_PORT_UDP 8888

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "[-] Invalid number of arguments" << std::endl;
        std::cout << "[*] Usage: ./client [-tcp|-udp]" << std::endl;
        return 1;
    }

    std::unique_ptr<Client> client;

    if (std::string(argv[1]) == "-tcp")
    {
        client = std::make_unique<ClientTCP>(DEFAULT_HOST, DEFAULT_PORT_TCP);
    }
    else if (std::string(argv[1]) == "-udp")
    {
        client = std::make_unique<ClientUDP>(DEFAULT_HOST, DEFAULT_PORT_UDP);
    }
    else
    {
        std::cout << "[-] Invalid argument" << std::endl;
        return 2;
    }

    if (!client)
    {
        std::cout << "[-] Failed to create client object" << std::endl;
        return 3;
    }

    if (!client->initialize())
    {
        std::cout << "[-] Failed to initialize" << std::endl;
        return 4;
    }

    while (true)
    {
        std::cout << "[*] Enter string:" << std::endl;

        std::string input_str;
        std::getline(std::cin, input_str);

        if (input_str == "exit")
            break;

        if (!client->send(input_str))
        {
            std::cout << "[-] Failed to send the message to the server" << std::endl;
            continue;
        }

        std::string reply;

        if (!client->receive(reply))
        {
            std::cout << "[-] Failed to receive the message from the server" << std::endl;
            continue;
        }

        std::cout << "[*] Server reply:" << std::endl;
        std::cout << reply << std::endl;
    }

    std::cout << "[*] Bye!" << std::endl;
    return 0;
}
