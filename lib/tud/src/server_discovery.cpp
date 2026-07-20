#include "../include/server_discovery.h"

#include <tablog_registry.h>
#include <tablog.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <thread>
#include <algorithm>
#include <optional>
#include <memory>

namespace tud {
    ServerDiscovery::ServerDiscovery(std::string interface, int inPort, int outPort, std::optional<std::string> identifier) {
        tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
        std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
        logger->configure("ServerUdpDiscovery", true);
        registry->registerLogger("ServerUdpDiscovery", logger);
        this->logger = logger;

        this->containerIP = getLocalIpAddress(interface);
        this->broadcastIP = getBroadcastIpAddress();

        this->inPort = inPort;
        this->outPort = outPort;

        if(identifier.has_value()) {
            this->identifier = identifier.value();
        }
    }

    void ServerDiscovery::discoveryCycle() {
        std::thread receiveDiscoveredCycleThread([this]() {
            receiveDiscoveredCycle();
        });
    
        int serverSocket;
        struct sockaddr_in broadcast{}, receiverAddress{};
        const int port = this->inPort;

        // Create socket
        if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            logger->log(tablog::ERROR, "Failed to create Socket!");
            return;
        }
        // Enable broadcast
        int broadcastBind = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, &broadcastBind, sizeof(broadcastBind)) < 0) {
            logger->log(tablog::ERROR, "Failed to enable broadcast!");
            close(serverSocket);
            return;
        }
        // Allow reuse
        int reuse = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            logger->log(tablog::ERROR, "Setsockopt failed!");
            close(serverSocket);
            return;
        }

        // clear garbage
        memset(&broadcast, 0, sizeof(broadcast));
        // prepare socket
        broadcast.sin_family = AF_INET;
        broadcast.sin_port = htons(port);

        if (inet_pton(AF_INET, broadcastIP.c_str(), &broadcast.sin_addr) <= 0) {
            logger->log(tablog::ERROR, "Invalid broadcast IP");
            close(serverSocket);
            return;
        }

        while (true) {
            std::string message = this->identifier + containerIP;
            if (sendMessageTo(serverSocket, broadcast, message.c_str()) != 0) {
                logger->log(tablog::ERROR, "Broadcast failed!");
                return;
            }
            usleep(100000);
        }

        receiveDiscoveredCycleThread.join();
    }

    void ServerDiscovery::receiveDiscoveredCycle() {
        int udpSocket;
        const int port = this->outPort; 
    
        udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket < 0) {
            logger->log(tablog::ERROR, "Create socket failed!");
            return;
        }

        int broadcast = 1;
        setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

        sockaddr_in nodeAddress{};
        nodeAddress.sin_family = AF_INET;
        nodeAddress.sin_addr.s_addr = inet_addr(this->containerIP.c_str());
        nodeAddress.sin_port = htons(port);

        if (bind(udpSocket, (struct sockaddr*)&nodeAddress, sizeof(nodeAddress)) < 0) {
            logger->log(tablog::ERROR, "UDP Socket bind failed!");
            return;
        }

        while (true) {
            std::string receivedMessage = receiveMessage(udpSocket);
            if (hasSameIdentifier(receivedMessage)) {
                std::string newAddress = stripIdentifier(receivedMessage);
                if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), newAddress) == discoveredAddresses.end()) {
                    discoveredAddresses.push_back(newAddress);
                }
            }
        }
    }
}
