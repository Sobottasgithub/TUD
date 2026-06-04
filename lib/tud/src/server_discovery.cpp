#include "../include/server_discovery.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <thread>

ServerDiscovery::ServerDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
    this->broadcastIP = getBroadcastIpAddress();
}

void ServerDiscovery::discoveryCycle() {
    std::thread receiveDiscoveredCycleThread([this]() {
        receiveDiscoveredCycle();
    });
    
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
        usleep(100000);
    }

    receiveDiscoveredCycleThread.join();
}

void ServerDiscovery::receiveDiscoveredCycle() {
    int udpSocket;
    const int port = 4001; 
    
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        std::wcout << "Create socket failed!" << std::endl;
        return;
    }

    int broadcast = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    sockaddr_in nodeAddress{};
    nodeAddress.sin_family = AF_INET;
    nodeAddress.sin_addr.s_addr = inet_addr(this->containerIP.c_str());
    nodeAddress.sin_port = htons(port);

    if (bind(udpSocket, (struct sockaddr*)&nodeAddress, sizeof(nodeAddress)) < 0) {
        std::wcout << "UDP Socket bind failed!" << std::endl;
        return;
    }

    while (true) {
        std::wcout << "Answere: "<< receiveMessage(udpSocket).c_str() << std::endl;
    }

}
