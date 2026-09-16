#include "../include/peer_discovery.h"

#include <tablog.h>
#include <tablog_registry.h>

namespace tud {
  PeerDiscovery::PeerDiscovery() {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
    logger->configure("PeerUdpDiscovery", true);
    registry->registerLogger("ClientUdpDiscovery", logger);
    this->logger = logger;
    
    this->logger->log(tablog::DEBUG, "PeerDiscovery");
  }
}
