#ifndef UDP_STRUCTURE_H
#define UDP_STRUCTURE_H

#include <string>
#include <netinet/in.h>
#include <vector>
#include <mutex>

namespace tud {
    class UdpStructure
    {
        public:
            std::vector<std::string> getDiscoveredAddresses();
            void removeDiscoveredAddress(std::string address);

            std::string getLocalIpAddress(std::string interface);
            std::string getBroadcastIpAddress();
            bool isValidIpV4(std::string &ipString);
        
        protected:
            int sendMessageTo(int socket, const sockaddr_in& broadcast, std::string payload);
            std::string receiveMessage(int socket);

            bool hasSameIdentifier(std::string ipString);
            std::string stripIdentifier(std::string ipString);

            std::vector<std::string> discoveredAddresses = {};
            std::mutex mtx;
            std::string containerIP;
            int inPort;
            int outPort;

            std::string identifier = "tud";

        private:
            std::string identifierEscapeRegex(const std::string& text);
    };
}
#endif
