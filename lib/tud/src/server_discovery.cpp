#include "../include/server_discovery.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

ServerDiscovery::ServerDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
    this->broadcastIP = getBroadcastIpAddress();
}

void ServerDiscovery::discoveryCycle() {
    int serverSocket;
    struct sockaddr_in broadcast{}, receiverAddress{};
    const int port = 4000;

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::wcout << "Failed to create Socket!" << std::endl;
        return;
    }
    // Enable broadcast
    int broadcastBind = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, &broadcastBind, sizeof(broadcastBind)) < 0) {
        std::wcout << "Failed to enable broadcast!" << std::endl;
        close(serverSocket);
        return;
    }
    // Allow reuse
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::wcout << "Setsockopt failed!" << std::endl;
        close(serverSocket);
        return;
    }

    // clear garbage
    memset(&broadcast, 0, sizeof(broadcast));
    // prepare socket
    broadcast.sin_family = AF_INET;
    broadcast.sin_port = htons(port);

    if (inet_pton(AF_INET, broadcastIP.c_str(), &broadcast.sin_addr) <= 0) {
        std::wcout << "Invalid broadcast IP" << std::endl;
        close(serverSocket);
        return;
    }

    while (true) {
        if (sendMessageTo(serverSocket, broadcast, containerIP.c_str()) != 0) {
            std::wcout << "Broadcast failed!" << std::endl;
            return;
        }
        std::wcout << "Broadcast send!" << std::endl;
        usleep(100000);
    }
}
