#include "../include/client_discovery.h"

#include <iostream>

ClientDiscovery::ClientDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
}

void discoveryCycle() {
    std::wcout << "Start udp discovery..." << std::endl;
}
