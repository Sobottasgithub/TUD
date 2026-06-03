#include "../include/server_discovery.h"

#include <iostream>

ServerDiscovery::ServerDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
    this->broadcastIP = getBroadcastIpAddress();
}

void discoveryCycle() {
  
}
