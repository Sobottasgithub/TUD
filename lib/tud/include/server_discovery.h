#ifndef SERVER_DISCOVERY_H
#define SERVER_DISCOVERY_H

#include "udp_structure.h"

#include <string>
#include <optional>

namespace tud {
  class ServerDiscovery: public UdpStructure
  {
      public:
        ServerDiscovery(std::string interface, int inPort, int outPort, std::optional<std::string> identifier);
        void discoveryCycle();

      private:
        std::string broadcastIP;

        void receiveDiscoveredCycle();

  };
}

#endif
