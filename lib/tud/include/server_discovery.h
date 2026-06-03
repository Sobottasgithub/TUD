#ifndef SERVER_DISCOVERY_H
#define SERVER_DISCOVERY_H

#include "networking.h"

#include <string>

class ServerDiscovery: public Networking
{
    public:
      ServerDiscovery(std::string interface);
      void discoveryCycle();

    private:
      std::string broadcastIP;
};

#endif
