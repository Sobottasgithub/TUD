#include "../include/peer_discovery.h"

#include <string>
#include <tablog.h>
#include <tablog_registry.h>

#include <memory>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include<bits/stdc++.h>

namespace tud {
  PeerDiscovery::PeerDiscovery(std::string interface, int port) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
    logger->configure("PeerUdpDiscovery", true);
    registry->registerLogger("ClientUdpDiscovery", logger);
    this->logger = logger;

    this->containerIP = getLocalIpAddress(interface);
    this->broadcastIP = getBroadcastIpAddress();

    this->port = port;

    this->identifier = "tud-peer-" + std::to_string(generateSeed()) + "-";
  }
  
  void PeerDiscovery::discoveryCycle() {
    std::thread discoveryBroadcastCycleThread([this]() {
        discoveryBroadcastCycle();
    });

    std::thread discoveryResponseCycleThread([this]() {
        discoveryResponseCycle();
    });

    if (discoveryBroadcastCycleThread.joinable())
        discoveryBroadcastCycleThread.join();

    if (discoveryResponseCycleThread.joinable())
        discoveryResponseCycleThread.join();
  }

  std::vector<std::string> PeerDiscovery::getDiscoveredAddresses() {
    return getDiscovered(0);
  }

  std::vector<std::string> PeerDiscovery::getDiscoveredIdentifiers() {
    return getDiscovered(1);
  }

  std::map<std::string, std::string> PeerDiscovery::getDiscoveredPeers() {
    std::lock_guard<std::mutex> lock(mtx);
    return this->discoveredPeers;
  }

  std::vector<std::string> PeerDiscovery::getDiscovered(bool option) {
    std::lock_guard<std::mutex> lock(mtx);
    
    std::vector<std::string> ips;
    for (auto iterator = this->discoveredPeers.begin();
         iterator != this->discoveredPeers.end();
         ++iterator) {
        if (option) {
            ips.push_back(iterator->first);
        } else {
            ips.push_back(iterator->second);
        }
    }

    return ips;
  }
  
  void PeerDiscovery::removeDiscoveredAddress(std::string address) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto iterator = this->discoveredPeers.begin();
         iterator != this->discoveredPeers.end();
         ++iterator) {
        if (iterator->second == address) {
            this->discoveredPeers.erase(iterator);
            return;
        }
    } 
  }

  void PeerDiscovery::discoveryResponseCycle() {
    const int port = this->port; 

    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        logger->log(tablog::ERROR, "Create socket failed!");
        return;
    }

    int opt = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #ifdef SO_REUSEPORT
    setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    #endif

    sockaddr_in nodeAddress{};
    nodeAddress.sin_family = AF_INET;
    nodeAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    nodeAddress.sin_port = htons(port);

    if (bind(udpSocket, (struct sockaddr*)&nodeAddress, sizeof(nodeAddress)) < 0) {
        logger->log(tablog::ERROR, "UDP Socket bind failed!");
        close(udpSocket);
        return;
    }

    while (true) {
        std::string receivedMessage = receiveMessage(udpSocket);

        if (receivedMessage.empty()) {
            usleep(10000);
            continue;
        }

        if (!hasSameIdentifier(receivedMessage)) {
            std::tuple<std::string, std::string> messageParts = stripUniqueIdentifier(receivedMessage);
            auto& [identifier, peerIP] = messageParts;
            if (isValidIpV4(peerIP)) {
                if (discoveredPeers.find(identifier) == discoveredPeers.end()) {
                    discoveredPeers[identifier] = peerIP;
                }
            }
        }
    }

    close(udpSocket);
  }

  void PeerDiscovery::discoveryBroadcastCycle() {
    const int port = this->port;

    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        logger->log(tablog::ERROR, "Failed to create broadcast socket!");
        return;
    }

    int broadcastOpt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, &broadcastOpt, sizeof(broadcastOpt)) < 0) {
        logger->log(tablog::ERROR, "Failed to enable broadcast!");
        close(serverSocket);
        return;
    }

    int reuseOpt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseOpt, sizeof(reuseOpt)) < 0) {
        logger->log(tablog::ERROR, "Failed to set SO_REUSEADDR!");
        close(serverSocket);
        return;
    }

    sockaddr_in broadcastAddress{};
    broadcastAddress.sin_family = AF_INET;
    broadcastAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, broadcastIP.c_str(), &broadcastAddress.sin_addr) <= 0) {
        logger->log(tablog::ERROR, "Invalid broadcast IP address: " + broadcastIP);
        close(serverSocket);
        return;
    }

    std::string message = this->identifier + containerIP;
    while (true) {        
        if (sendMessageTo(serverSocket, broadcastAddress, message.c_str()) != 0) {
            logger->log(tablog::ERROR, "Broadcast send failed!");
        }
        usleep(100000);
    }

    close(serverSocket);
  }

  std::tuple<std::string, std::string> PeerDiscovery::stripUniqueIdentifier(std::string peerMessage) {
    size_t firstDash = peerMessage.find('-');
    if (firstDash == std::string::npos) {
        return {};
    }

    size_t secondDash = peerMessage.find('-', firstDash + 1);
    if (secondDash == std::string::npos) {
        return {};
    }

    size_t thirdDash = peerMessage.find('-', secondDash + 1);
    if (thirdDash == std::string::npos) {
        return {};
    }

    std::string identifier = peerMessage.substr(secondDash + 1, thirdDash - (secondDash + 1));
    std::string ipAddress  = peerMessage.substr(thirdDash + 1);
    
    return make_tuple(identifier, ipAddress);
  }
}
