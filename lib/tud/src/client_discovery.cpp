#include "../include/client_discovery.h"

#include <iostream>

ClientDiscovery::ClientDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
}

void ClientDiscovery::discoveryCycle() {
    while (true) {
        std::wcout << "Start udp discovery..." << std::endl;
    }
}
