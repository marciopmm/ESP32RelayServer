#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

// Rename the credentials.sample.h file to credentials.h and 
// edit it according to your router configuration
#include "credentials.h"

#define DEBUG 0
#define SERIAL_BAUDRATE 115200
#define PIN23 23
#define OBLED 2

#if DEBUG == 1
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug(x)
  #define debugln(x)
#endif

WebServer server(80);

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 30);

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

String ssid = "<not connected>";
String localIp = "<not connected>";

void blinkingLed() {
  switch (WiFi.status())
  {
    case (WL_CONNECTED):
      digitalWrite(OBLED, HIGH);
      break;
    default:
      delay(250);
      digitalWrite(OBLED, HIGH);
      delay(250);
      digitalWrite(OBLED, LOW);
      break;
  }
}

void wifiSetup() {

  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Scan
  debugln(F("Scan WiFi start"));

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  debugln(F("Scan WiFi done"));

  if (n == 0) {
    debugln(F("No networks found"));
  } else {
    debug(n);
    debugln(F(" networks found"));

    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      debugln("STA Failed to configure");
    } else {        
      for (int i = 0; i < n; i++) {
        // Connect to the current network found
        debug("[WIFI] Connecting to ");
        debugln(WiFi.SSID(i));
        WiFi.begin(WiFi.SSID(i).c_str(), WIFI_PASS);
    
        // Wait
        int count = 0;
        while (WiFi.status() != WL_CONNECTED) {
          if (count < 30) {
            debug(".");
            blinkingLed();
            count++;
          } else {
            break;
          }
        }
        debugln("");

        if (WiFi.status() == WL_CONNECTED) {
          // Connected!
          ssid = WiFi.SSID(i);
          localIp = WiFi.localIP().toString();
          debug(F("[WIFI] STATION Mode, SSID: "));
          debug(ssid);
          debug(F(" , IP address: "));
          debugln(localIp);
          break;
        }            
      }
    }
  }
  debugln("");
}

void handleRoot() {
  String buffer;
  buffer += "[WIFI] STATION Mode, SSID: ";
  buffer += ssid;
  buffer += ", IP address: ";
  buffer += localIp;
  server.send(200, F("text/plain"), buffer);
  debugln(buffer);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  debugln(F("NOT FOUND"));
}

void handleRelayOn() {
  digitalWrite(PIN23, HIGH);
  server.send(200, "text/plain", "Relay ON");
  debugln(F("Relay ON"));
}

void handleRelayOff() {
  digitalWrite(PIN23, LOW);
  server.send(200, "text/plain", "Relay OFF");
  debugln(F("Relay OFF"));
}

void serverSetup() {

    server.on("/", handleRoot);

    server.on("/relayOn", handleRelayOn);

    server.on("/relayOff", handleRelayOff);

    server.onNotFound(handleNotFound);    

    // Start the server
    server.begin();
    debug(F("HTTP server started on IP: "));
    debug(localIp);
    debugln(F(", PORT: 80"));
}

void setup() {

  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  debugln("");

  // RELAY
  pinMode(PIN23, OUTPUT);
  pinMode(OBLED, OUTPUT);
  digitalWrite(PIN23, HIGH);

  // Wifi
  wifiSetup();

  // Web server
  serverSetup();
}

void loop() {
  blinkingLed();
  server.handleClient();
  delay(2); //allow the cpu to switch to other tasks
}
