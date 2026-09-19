#ifndef PEER_DISCOVERY_H
#define PEER_DISCOVERY_H

#include "networking.h"

#include <bits/stdc++.h>
#include <unordered_map>

namespace tud {
  class PeerDiscovery: public Networking {
    public:
      PeerDiscovery(std::string interface, int port);
      void discoveryCycle();

      std::vector<std::string> getDiscoveredAddresses();
      std::vector<std::string> getDiscoveredIdentifiers();
      std::map<std::string, std::string> getDiscoveredPeers();
      
      void removeDiscoveredAddress(std::string address);
      
    private:
      std::string broadcastIP;
      int port;

      std::vector<std::string> getDiscovered(bool option);
      
      std::map<std::string, std::string> discoveredPeers;

      void discoveryResponseCycle();
      void discoveryBroadcastCycle();

      std::tuple<std::string, std::string> stripUniqueIdentifier(std::string peerMessage);
  };
}

#endif
