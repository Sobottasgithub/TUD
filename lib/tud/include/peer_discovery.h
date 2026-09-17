#ifndef PEER_DISCOVERY_H
#define PEER_DISCOVERY_H

#include "networking.h"

namespace tud {
  class PeerDiscovery: public Networking {
    public:
      PeerDiscovery(std::string interface,
                    int broadcastPort,
                    int responsePort,
                    int registerPort,
                    std::optional<std::string> identifier);
    private:
      std::string broadcastIP;
      int broadcastPort;
      int responsePort;
      int registerPort;

      void discoveryResponseCycle();
      void discoveryBroadcastCycle();
      void discoveredRegisterCycle();
      
  };
}

#endif
