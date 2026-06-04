#include "client_discovery.h"

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

    auto clientDiscovery = std::make_shared<ClientDiscovery>(interface, 4000, 4001);
    std::thread clientDiscoveryThread([clientDiscovery]() {
      clientDiscovery->discoveryCycle();
    });

    std::wcout << "~~ Discovered Addresses ~~" << std::endl;
    std::vector<std::string> discoveredAddresses;
    while(true) {
      std::vector<std::string> newDiscoveries = clientDiscovery->getDiscoveredAdresses();
      for (int index = 0; index < newDiscoveries.size(); index++) {
        if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newDiscoveries[index]) == discoveredAddresses.end()) {
          std::wcout << "--> " << newDiscoveries[index].c_str() << std::endl;
          discoveredAddresses.push_back(newDiscoveries[index]);
        }
      }
    }
    
    clientDiscoveryThread.join();
    
    return 0;
}
