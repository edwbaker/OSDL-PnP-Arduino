
#if defined(ARDUINO) && ARDUINO > 18
#include <SPI.h>
#endif
#include <Ethernet.h>
#include <EthernetBonjour.h>
#include "sensor.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const PROGMEM sensor_t sensors[] = {
  {1, "Ultrasupersonic Sensor._sensor", "http://osdl.myspecies.info/node/4"}, 
  {2, "InfraRed Sensor._sensor", "http://osdl.myspecies.info/node/5"}
};

const int c_sensors = sizeof(sensors) / sizeof(sensor_t);

EthernetServer server(80);

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac);
  server.begin();
  EthernetBonjour.begin("arduino");
  for (int i = 0; i < c_sensors -1; i++) {
    Serial.print("Name: ");
    Serial.println(sensors[i].name);
    //EthernetBonjour.addServiceRecord(getString(sensors[i].name), 80, MDNSServiceTCP, "\x7path=/2");
  }
}

void loop()
{ 
  // This actually runs the Bonjour module. YOU HAVE TO CALL THIS PERIODICALLY,
  // OR NOTHING WILL WORK! Preferably, call it once per loop().
  EthernetBonjour.run();

  // The code below is just taken from the "WebServer" exmple in the Ethernet
  // library. The only difference here is that this web server gets announced
  // over Bonjour, but this happens in setup(). This just displays something
  // in the browser when you connect.
  EthernetClient client = server.available();
  if (client) {
    //Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.print("{\"sensor\":[{\"name\":\"");
          //client.print(getString("sensor_name"));
          client.print("\", \"data_url\":\"");
          //client.print(getString("sensor_data_url"));
          client.println("\"} ]}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //Serial.println("client disonnected");
  }
}
