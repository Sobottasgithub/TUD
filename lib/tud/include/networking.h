#ifndef NETWORKING_H
#define NETWORKING_H

#include <string>
#include <netinet/in.h>

class Networking
{
    protected:
        int sendMessageTo(int socket, const sockaddr_in& broadcast, std::string payload);
        std::string receiveMessage(int socket);
};

#endif
