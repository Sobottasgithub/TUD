#include "client_discovery.h"

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
    logger->configure("TUD-client", true);
    
    std::string interface;
    std::cout << "Interface: ";
    std::cin >> interface;

    auto clientDiscovery = std::make_shared<tud::ClientDiscovery>(interface, 4000, 4001, "tud");
    std::thread clientDiscoveryThread([clientDiscovery]() {
      clientDiscovery->discoveryCycle();
    });

    logger->log(tablog::INFO, "~~ Discovered Addresses ~~");
    std::vector<std::string> discoveredAddresses;
    while(true) {
      std::vector<std::string> newDiscoveries = clientDiscovery->getDiscoveredAddresses();
      for (int index = 0; index < newDiscoveries.size(); index++) {
        if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newDiscoveries[index]) == discoveredAddresses.end()) {
          logger->log(tablog::INFO, "--> " + newDiscoveries[index]);
          discoveredAddresses.push_back(newDiscoveries[index]);
        }
      }
    }
    
    clientDiscoveryThread.join();
    
    return 0;
}
