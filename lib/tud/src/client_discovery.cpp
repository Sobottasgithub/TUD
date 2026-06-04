#include "../include/client_discovery.h"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>

ClientDiscovery::ClientDiscovery(std::string interface) {
    this->containerIP = getLocalIpAddress(interface);
}

void ClientDiscovery::discoveryCycle() {
    std::wcout << "Start udp discovery..." << std::endl;

    // Udp receive Socket
    int udpSocket;
    const int port = 4000; 
    char buffer[1024];
    
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        std::wcout << "Create socket failed!" << std::endl;
        return;
    }

    int broadcast = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    sockaddr_in nodeAddress{};
    nodeAddress.sin_family = AF_INET;
    nodeAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    nodeAddress.sin_port = htons(port);

    if (bind(udpSocket, (struct sockaddr*)&nodeAddress, sizeof(nodeAddress)) < 0) {
        std::wcout << "UDP Socket bind failed!" << std::endl;
        return;
    }

    // UDP send socket
    int udpSendSocket;
    struct sockaddr_in serverAddress{}, receiverAddress{};
    const int sendPort = 4001;

    // Create socket
    if ((udpSendSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::wcout << "Failed to create Socket!" << std::endl;
        return;
    }
    // Allow reuse
    int reuse = 1;
    if (setsockopt(udpSendSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::wcout << "Setsockopt failed!" << std::endl;
        return;
    }

    
    std::wcout << "Listening on UDP port " << port << std::endl;
    while (true) {
        // Get UDP Discovery packet
        std::string masterIP = receiveMessage(udpSocket);
        if (isValidIpV4(masterIP)) {
            std::wcout << "Discovered ip: " << masterIP.c_str() << std::endl;

            // clear garbage
            memset(&serverAddress, 0, sizeof(serverAddress));
            // prepare socket
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(sendPort);

            if (inet_pton(AF_INET, masterIP.c_str(), &serverAddress.sin_addr) <= 0) {
                std::wcout << "Invalid broadcast IP" << std::endl;
                return;
            }
            
            if (sendMessageTo(udpSendSocket, serverAddress, containerIP.c_str()) != 0) {
                std::wcout << "Broadcast failed!" << std::endl;
                return;
            }
        }
    }
}
