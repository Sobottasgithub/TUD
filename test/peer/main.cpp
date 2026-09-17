#include "peer_discovery.h"

#include <stdexcept>
#include <string>
#include <memory>
#include <optional>

#include <tablog_registry.h>
#include <tablog.h>

using namespace tud;

std::string getArg(int argc, char *argv[], std::string argumentName) {
  for(int index = 0; index < argc; index++) {
    if (std::string(argv[index]).rfind(argumentName, 0) == 0) {
      std::string argument = argv[index+1];
      return argument;
    }
  }
  std::string errorMessage = "Unable to find " + argumentName;
  throw std::invalid_argument(errorMessage);
}

int getPort(int argc, char *argv[], std::string argumentName, std::string alternativArgumentName) {
  try {
    std::string stringPort = getArg(argc, argv, argumentName);
    // WARNING: it is not checked if stringPort is an int because it is supposed to throw an error
    // when a faulty port is provided!
    return std::stoi(stringPort);
  } catch (const std::invalid_argument& invalidArgument) {
    std::string stringPort = getArg(argc, argv, alternativArgumentName);
    // WARNING: it is not checked if stringPort is an int because it is supposed to throw an error
    // when a faulty port is provided!
    return std::stoi(stringPort);
  }
}

int main(int argc, char *argv[]) {  
  std::string interface = getArg(argc, argv, "--interface");

  int broadcastPort = getPort(argc, argv, "--broadcastPort", "-bP");
  int responsePort = getPort(argc, argv, "--responsePort", "-rsP");
  int registerPort = getPort(argc, argv, "--registerPort", "-rgP");
  
  tud::PeerDiscovery peerDiscovery(interface, broadcastPort, responsePort, registerPort, std::nullopt);
}

