# TUD
The ```Tablo Udp Discovery``` module can be used to easely implement UDP discovery into your project.

To use TUD in your project you need to include it first.
After including TUD to your project you can implement the server like this:
```cpp
#include "server_discovery.h"
```

```cpp
auto serverDiscovery = std::make_shared<ServerDiscovery>(interface, 4000, 4001);
std::thread serverDiscoveryThread([serverDiscovery]() {
  serverDiscovery->discoveryCycle();
});
```
And the client like this:
```cpp
#include "client_discovery.h"
```

```cpp
auto clientDiscovery = std::make_shared<ClientDiscovery>(interface, 4000, 4001);
std::thread clientDiscoveryThread([clientDiscovery]() {
  clientDiscovery->discoveryCycle();
});
```
Both constructors want:
1. your internet interface (string)
2. your in port (int)
3. your out port (int)

Then you can use it with these functions:
```cpp
std::vector<std::string> getDiscoveredAdresses();
void removeDiscoveredAddress(std::string address);

std::string getLocalIpAddress(std::string interface);
std::string getBroadcastIpAddress();
bool isValidIpV4(std::string &ipString);
```
