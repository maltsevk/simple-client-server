#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

#include "server_tcp.h"
#include "server_udp.h"

#define DEFAULT_PORT_TCP 7777
#define DEFAULT_PORT_UDP 8888

std::atomic_bool runned;

void signal_handler(int signum)
{
   std::cout << std::endl << "[*] Interrupt signal " << signum << " received" << std::endl;

   ::runned = false;
}

int main(int argc, char** argv)
{
    ::runned = true;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // TCP server --------------------------------------------------------------
    ServerTCP server_tcp(DEFAULT_PORT_TCP);

    if (!server_tcp.initialize())
    {
        std::cout << "[-] Failed to initialize TCP server" << std::endl;
        return 1;
    }

    std::cout << "[*] Starting TCP server thread..." << std::endl;
    std::thread server_tcp_thread([&server_tcp](){ server_tcp.run(); });
    // -------------------------------------------------------------------------

    // UDP server --------------------------------------------------------------
    ServerUDP server_udp(DEFAULT_PORT_UDP);

    if (!server_udp.initialize())
    {
        std::cout << "[-] Failed to initialize UDP server" << std::endl;
        std::cout << "[*] Stopping TCP server thread..." << std::endl;
        server_tcp.stop();
        return 1;
    }

    std::cout << "[*] Starting UDP server thread..." << std::endl;
    std::thread server_udp_thread([&server_udp](){ server_udp.run(); });
    // -------------------------------------------------------------------------

    while (::runned)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // TCP server thread -------------------------------------------------------
    std::cout << "[*] Stopping TCP server thread..." << std::endl;
    server_tcp.stop();
    // -------------------------------------------------------------------------

    // UDP server thread -------------------------------------------------------
    std::cout << "[*] Stopping UDP server thread..." << std::endl;
    server_udp.stop();
    // -------------------------------------------------------------------------

    std::cout << "[*] Waiting for TCP server thread..." << std::endl;
    server_tcp_thread.join();

    std::cout << "[*] Waiting for UDP server thread..." << std::endl;
    server_udp_thread.join();

    std::cout << "Bye!" << std::endl;
    return 0;
}
