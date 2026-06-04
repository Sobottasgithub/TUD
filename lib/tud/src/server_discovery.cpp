#include "../include/server_discovery.h"

#include <iostream>

ServerDiscovery::ServerDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
    this->broadcastIP = getBroadcastIpAddress();
}

void ServerDiscovery::discoveryCycle() {
    while (true) {
        std::wcout << "Start udp discovery..." << std::endl;
    }
}
