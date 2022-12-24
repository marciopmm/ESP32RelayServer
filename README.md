# ESP32 Relay Server
 
This code must be inserted into an ESP32 Controller.
It creates an Web Server at IP 192.168.1.30 (you may change it, if you want) and looks for a nearby WiFi connection. To work properlly, you must create a _credentials.h_ file to define the network password (the _credentials.sample.h_ file must be used as a model to _credentials.h_). The ESP32 will use this password to connect to WiFi network. 
In the mean time, the onboard led will blink until it finds an wireless network that fits the given password (set in _credentials.h_).

## Web Server Methods
- / (root)  - Gives the SSID connected to ESP32
- /relayOn  - Turns the relay ON
- /relayOff - Turns the relay OFFgit 