#include "client_discovery.h"

#include <iostream>
#include <string>
#include <thread>

int main() {
    std::string interface;
    std::wcout << "Interface: ";
    std::cin >> interface;
      
    ClientDiscovery clientDiscovery(interface);
    std::thread clientDiscoveryThread([&clientDiscovery]() {
        clientDiscovery.discoveryCycle();
    });
    clientDiscoveryThread.join();
    
    return 0;
}
