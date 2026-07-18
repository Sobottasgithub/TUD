#include "server_discovery.h"

#include <tablog.h>

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>

using namespace tud;

int main() {
    tablog::Tablog* logger = &tablog::Tablog::getInstance();
    logger->configure("TUD-server", true);

    std::string interface;
    std::cout << "Interface: ";
    std::cin >> interface;
      
    auto serverDiscovery = std::make_shared<tud::ServerDiscovery>(interface, 4000, 4001, "tud");
    std::thread serverDiscoveryThread([serverDiscovery]() {
      serverDiscovery->discoveryCycle();
    });

    logger->log(tablog::INFO, "~~ Discovered Addresses ~~");
    std::vector<std::string> discoveredAddresses;
    while(true) {
      std::vector<std::string> newDiscoveries = serverDiscovery->getDiscoveredAddresses();
      for (int index = 0; index < newDiscoveries.size(); index++) {
        if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newDiscoveries[index]) == discoveredAddresses.end()) {
          logger->log(tablog::INFO, "--> " + newDiscoveries[index]);
          discoveredAddresses.push_back(newDiscoveries[index]);
        }
      }
    }

    serverDiscoveryThread.join();
    
    return 0;
}
