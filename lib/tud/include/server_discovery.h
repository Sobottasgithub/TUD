#ifndef SERVER_DISCOVERY_H
#define SERVER_DISCOVERY_H

#include "networking.h"

class ServerDiscovery: public Networking
{
    public:
      ServerDiscovery();
      void discoveryCycle();
};

#endif
