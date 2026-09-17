#ifndef PEER_DISCOVERY_H
#define PEER_DISCOVERY_H

#include "networking.h"

namespace tud {
  class PeerDiscovery: public Networking {
    public:
      PeerDiscovery(std::string interface, int port);
      
    private:
      std::string broadcastIP;
      int port;

      void discoveryResponseCycle();
      void discoveryBroadcastCycle();      
  };
}

#endif
