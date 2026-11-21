#ifndef SERVER_DISCOVERY_H
#define SERVER_DISCOVERY_H

#include "networking.h"

#include <string>
#include <optional>

namespace tud {
  class ServerDiscovery: public Networking
  {
      public:
        ServerDiscovery(std::string interface, int inPort, int outPort, std::optional<std::string> identifier, const int interval = 1000);
        void discoveryCycle();

      private:
        std::string broadcastIP;
        int interval = 0;

        void receiveDiscoveredCycle();

  };
}

#endif
