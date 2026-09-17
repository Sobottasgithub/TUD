#include "../include/peer_discovery.h"

#include <string>
#include <tablog.h>
#include <tablog_registry.h>

#include <algorithm>
#include <memory>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

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

        if (hasSameIdentifier(receivedMessage)) {
            this->logger->log(tablog::CRITICAL, "-->" + receivedMessage);
            std::string masterIP = stripIdentifier(receivedMessage);

            if (isValidIpV4(masterIP)) {
                if (std::find(discoveredAddresses.begin(), discoveredAddresses.end(), masterIP) == discoveredAddresses.end()) {
                    discoveredAddresses.push_back(masterIP);
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
}
