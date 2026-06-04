#include "server_discovery.h"

#include <iostream>
#include <string>
#include <thread>

int main() {
    std::string interface;
    std::wcout << "Interface: ";
    std::cin >> interface;
      
    ServerDiscovery serverDiscovery(interface);
    std::thread serverDiscoveryThread([&serverDiscovery]() {
        serverDiscovery.discoveryCycle();
    });
    serverDiscoveryThread.join();
    
    return 0;
}
