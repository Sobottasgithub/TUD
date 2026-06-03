#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>
#include <netinet/in.h>
#include <vector>
#include <mutex>

class Networking
{
    public:
        std::vector<std::string> getDiscoveredAdresses();
        void removeDiscoveredAddress(std::string address);
        
    protected:
        int sendMessageTo(int socket, const sockaddr_in& broadcast, std::string payload);
        std::string receiveMessage(int socket);

        std::vector<std::string> discoveredAddresses = {};
        std::mutex mtx;
};

#endif
