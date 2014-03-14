
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetBonjour.h>
#include "osdl.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const PROGMEM char device_name[] = "EdSensor";
const PROGMEM OSDLsensor sensors[] = {
  {"1", "Ultrasupersonic Sensor", "http://osdl.myspecies.info/node/4", "/s/1"}, 
  {"2", "InfraRed Sensor", "http://osdl.myspecies.info/node/5", "/s/2"}
};
const PROGMEM OSDLaction actions[] = {
  {1, "Take a photo", "http://gooogle.com/takeaphoto", "/a/1"}
};

const int c_sensors = sizeof(sensors) / sizeof(OSDLsensor);
const int c_actions = sizeof(actions) / sizeof(OSDLaction);

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);
  server.begin();
  EthernetBonjour.begin(OSDLsafeDeviceName(OSDLgetString(device_name)));
  EthernetBonjour.addServiceRecord(
    strcat(OSDLsafeDeviceName(OSDLgetString(device_name)), "._sensor"), 
    80, 
    MDNSServiceTCP
    //"\xdpath=/osdl.js"
  );
}

void loop() { 
  EthernetBonjour.run();
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
	  client.println("Refresh: 5");
          client.println();
          client.print("{\"sensors\":[");
            for (int i = 0; i < c_sensors; i++) {
              client.print("{\"");
              client.print(OSDLgetString(sensors[i].id));
                client.print("\":[");
                  client.print("{\"name\":\""); client.print(OSDLgetString(sensors[i].name)); client.print("\"},");
                  client.print("{\"data_url\":\""); client.print(OSDLgetString(sensors[i].data_url)); client.print("\"},");
                  client.print("{\"request_path\":\""); client.print(OSDLgetString(sensors[i].request_path)); client.print("\"}");
                client.print("]");
             client.print("}");
             if (i != c_sensors - 1) {
               client.print(",");
             }
            }
          client.println("]}");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}
