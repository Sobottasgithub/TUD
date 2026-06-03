#include "../include/networking.h"

#include <iostream>
#include <sys/poll.h>
#include <netinet/in.h>

int Networking::sendMessageTo(int socket, const sockaddr_in& broadcast, std::string payload) {
  if (sendto(socket, payload.data(), payload.size(), 0, (struct sockaddr*)&broadcast, sizeof(broadcast)) < 0) {
      std::wcout << "buffer: Sendto Failed!" << std::endl;
      return -1;
  }

  return 0;
}

std::string Networking::receiveMessage(int socket) {
  std::string data;
  pollfd pfd{};
  pfd.fd = socket;
  pfd.events = POLLIN;

  int ret = poll(&pfd, 1, 10000);

  int bufferSize = 1024;
  if (ret > 0 && (pfd.revents & POLLIN)) {
      char* buffer = new char[bufferSize];
      ssize_t size = recv(socket, buffer, bufferSize, 0);
      
      data = buffer;
    
      if (size <= 0) return data;
      return data;
  }
  return data;
}
