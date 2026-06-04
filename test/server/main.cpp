#include "server_discovery.h"

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>

int main() {
    std::string interface;
    std::wcout << "Interface: ";
    std::cin >> interface;
      
    auto serverDiscovery = std::make_shared<ServerDiscovery>(interface, 4000, 4001);
    std::thread serverDiscoveryThread([serverDiscovery]() {
      serverDiscovery->discoveryCycle();
    });

    std::wcout << "~~ Discovered Addresses ~~" << std::endl;
    std::vector<std::string> discoveredAddresses;
    while(true) {
      std::vector<std::string> newDiscoveries = serverDiscovery->getDiscoveredAdresses();
      for (int index = 0; index < newDiscoveries.size(); index++) {
        if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newDiscoveries[index]) == discoveredAddresses.end()) {
          std::wcout << "--> " << newDiscoveries[index].c_str() << std::endl;
          discoveredAddresses.push_back(newDiscoveries[index]);
        }
      }
    }

    serverDiscoveryThread.join();
    
    return 0;
}
