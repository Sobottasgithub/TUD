#include "../include/client_discovery.h"

#include <tablog.h>

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <algorithm>
#include <optional>

namespace tud {
    ClientDiscovery::ClientDiscovery(std::string interface, int inPort, int outPort, std::optional<std::string> identifier) {
        logger->configure("ClientUdpDiscovery", true);
        
        this->containerIP = getLocalIpAddress(interface);

        this->inPort = inPort;
        this->outPort = outPort;

        if(identifier.has_value()) {
            this->identifier = identifier.value();
        }
    }

    void ClientDiscovery::discoveryCycle() {
        // Udp receive Socket
        int udpSocket;
        const int port = this->inPort; 
        char buffer[1024];
    
        udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udpSocket < 0) {
            logger->log(tablog::ERROR, "Create socket failed!");
            return;
        }

        int broadcast = 1;
        setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

        sockaddr_in nodeAddress{};
        nodeAddress.sin_family = AF_INET;
        nodeAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        nodeAddress.sin_port = htons(port);

        if (bind(udpSocket, (struct sockaddr*)&nodeAddress, sizeof(nodeAddress)) < 0) {
            logger->log(tablog::ERROR, "UDP Socket bind failed!");
            return;
        }

        // UDP send socket
        int udpSendSocket;
        struct sockaddr_in serverAddress{}, receiverAddress{};
        const int sendPort = this->outPort;

        // Create socket
        if ((udpSendSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            logger->log(tablog::ERROR, "Failed to create Socket!");
            return;
        }
        // Allow reuse
        int reuse = 1;
        if (setsockopt(udpSendSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            logger->log(tablog::ERROR, "Setsockopt failed!");
            return;
        }
    
        while (true) {
            // Get UDP Discovery packet
            std::string receivedMessage = receiveMessage(udpSocket);
            if (hasSameIdentifier(receivedMessage)) {
                std::string masterIP = stripIdentifier(receivedMessage);
                if (isValidIpV4(masterIP)) {
                    if(std::find(discoveredAddresses.begin(), discoveredAddresses.end(), masterIP) == discoveredAddresses.end()) {
                        discoveredAddresses.push_back(masterIP);
                    }

                    // clear garbage
                    memset(&serverAddress, 0, sizeof(serverAddress));
                    // prepare socket
                    serverAddress.sin_family = AF_INET;
                    serverAddress.sin_port = htons(sendPort);

                    if (inet_pton(AF_INET, masterIP.c_str(), &serverAddress.sin_addr) <= 0) {
                        logger->log(tablog::ERROR, "Invalid broadcast IP");
                        return;
                    }

                    std::string message = this->identifier + containerIP;
                    if (sendMessageTo(udpSendSocket, serverAddress, message.c_str()) != 0) {
                        logger->log(tablog::ERROR, "Broadcast failed!");
                        return;
                    }
                }
            }
        }
    }
}
