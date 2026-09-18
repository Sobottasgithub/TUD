#ifndef PEER_DISCOVERY_H
#define PEER_DISCOVERY_H

#include "networking.h"

#include <bits/stdc++.h>
#include <unordered_map>

namespace tud {
  class PeerDiscovery: public Networking {
    public:
      PeerDiscovery(std::string interface, int port);
      
    private:
      std::string broadcastIP;
      int port;
      std::map<std::string, std::string> discoveredPeers;

      void discoveryResponseCycle();
      void discoveryBroadcastCycle();

      std::tuple<std::string, std::string> stripUniqueIdentifier(std::string peerMessage);
  };
}

#endif
