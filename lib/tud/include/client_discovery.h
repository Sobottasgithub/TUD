#ifndef CLIENT_DISCOVERY_H
#define CLIENT_DISCOVERY_H

#include "networking.h"

#include <string>
#include <optional>

class ClientDiscovery: public Networking
{
    public:
      ClientDiscovery(std::string interface, int inPort, int outPort, std::optional<std::string> identifier);
      void discoveryCycle();
};

#endif
