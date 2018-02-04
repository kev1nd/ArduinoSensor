// Arduino Data Collector
//
// Collects data and sends to ThingSpeak site for analysis
//
// Author: Kevin Dunwell
//
// Version 1.0.3
//
//
//  02/03/18  1.0.2   Changed SSID to VioletInternet for new hub
//  02/03/18  1.0.3   Add support for DHT11. Add support for Photo-resistor

#include <WiFiEsp.h>
#include <SimpleDHT.h>
// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;


// emulate Serial1 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

unsigned long waitTime = 30000; // Time to wait for a response
char ssid[] = "VioletInternet"; // Wifi SSID
char pass[] = "kjjj1997";     // Wifi Password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "api.thingspeak.com";
int tempPin = 0;                // Sensor pin for Thermister
int lightPin = 1;               // Sensor pin for light level

WiFiEspClient client;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);         // The port back to the PC, if connected
  Serial1.begin(9600);          // ESP8266 serial port

  WiFi.init(&Serial1);
  delay(1000);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }


  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  int lightLevel  = analogRead(lightPin);

  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.println("Read DHT11 failed");
    return;
  }

  String thermister = (String)getThermister();

  sendData(thermister, (String)temperature, (String)humidity, (String)lightLevel);

  digitalWrite(LED_BUILTIN, LOW);

  delay(30000);

}



void sendData(String field1, String field2, String field3, String field4) {

  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    // send the HTTP PUT request
    client.println("GET https://api.thingspeak.com/update?api_key=WZDTWH6PEE2G0YII&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4);
    //client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}



float getThermister() {
  int tempReading = analogRead(tempPin);
  // This is OK
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  // float tempF = (tempC * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit

  return tempC;
}

