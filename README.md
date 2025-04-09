# Wake-on-LAN
- A simple tool for sending Wake-on-LAN packets to remotely turn on computers over a network and Ping a specific device on the network.
## Description
- This project allows you to turn on computers remotely using Wake-on-LAN technology. It supports sending "magic packets" over the local network and scanning the network using Ping.
### Opportunities
- Sending WoL packets to a MAC address
- Support for broadcast requests
- Scanning a local network for a specific IP address
#### Instruction manual
- `cd build/wol-backend/`
- `cmake ..`
- `make`
- `./wol "your_mac_addr" "ip"`
