#ifndef Networking_H
#define Networking_H

#include <tablog.h>

#include <string>
#include <netinet/in.h>
#include <vector>
#include <mutex>
#include <memory>

namespace tud {
    class Networking
    {
        public:
            std::vector<std::string> getDiscoveredAddresses();
            void removeDiscoveredAddress(std::string address);

            std::string getLocalIpAddress(std::string interface);
            std::string getBroadcastIpAddress();
            bool isValidIpV4(std::string &ipString);
        
        protected:
            std::shared_ptr<tablog::Tablog> logger;
            
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
