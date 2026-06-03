#ifndef CLIENT_DISCOVERY_H
#define CLIENT_DISCOVERY_H

#include "networking.h"

#include <string>

class ClientDiscovery: public Networking
{
    public:
      ClientDiscovery(std::string interface);
      void discoveryCycle();
};

#endif
