#include "../include/peer_discovery.h"

#include <tablog.h>
#include <tablog_registry.h>

#include <algorithm>
#include <memory>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>

namespace tud {
  PeerDiscovery::PeerDiscovery(std::string interface,
                               int broadcastPort,
                               int responsePort,
                               int registerPort,
                               std::optional<std::string> identifier) {
    tablog::TablogRegistry* registry = &tablog::TablogRegistry::getInstance();
    std::shared_ptr<tablog::Tablog> logger = std::make_shared<tablog::Tablog>();
    logger->configure("PeerUdpDiscovery", true);
    registry->registerLogger("ClientUdpDiscovery", logger);
    this->logger = logger;

    this->containerIP = getLocalIpAddress(interface);

    this->broadcastPort = broadcastPort;
    this->responsePort = responsePort;
    this->registerPort = registerPort;

    if (identifier.has_value()) {
        this->identifier = identifier.value();
    } else {
        // TODO: gernerate custom identifier
    }

    std::thread discoveryBroadcastCycleThread([this]() {
        discoveryBroadcastCycle();
    });

    std::thread discoveryResponseCycleThread([this]() {
        discoveryResponseCycle();
    });

    std::thread discoveredRegisterCycleThread([this]() {
        discoveredRegisterCycle();
    });

    if (discoveryBroadcastCycleThread.joinable())
        discoveryBroadcastCycleThread.join();

    if (discoveryResponseCycleThread.joinable())
        discoveryResponseCycleThread.join();

    if (discoveredRegisterCycleThread.joinable())
        discoveredRegisterCycleThread.join();
  }

  void PeerDiscovery::discoveryResponseCycle() {
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

    void PeerDiscovery::discoveryBroadcastCycle() {
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
    }

    void PeerDiscovery::discoveredRegisterCycle() {
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
