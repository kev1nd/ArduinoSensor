// Arduino Data Collector
//
// Collects data and sends to ThingSpeak site for analysis
//
// Author: Kevin Dunwell
//
// Version 1.0.4
//
//
//  02/03/18  1.0.2   Changed SSID to VioletInternet for new hub
//  02/03/18  1.0.3   Add support for DHT11. Add support for Photo-resistor
//  02/03/18  1.0.4   Add WiFiEsp library and EEPROM storage for ssid/password

#include "thingspeak.h"
//#include <WiFiEsp.h>
#include <SimpleDHT.h>
#include <EEPROM.h>

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
char ssid[15] = ""; // Wifi SSID
char pass[15] = "";       // Wifi Password
char apikey[] = "WZDTWH6PEE2G0YII";
//int status = WL_IDLE_STATUS;    // the Wifi radio's status
//char server[] = "api.thingspeak.com";
int tempPin = 0;                // Sensor pin for Thermister
int lightPin = 1;               // Sensor pin for light level

//WiFiEspClient client;
ThingSpeak thingspeak(apikey);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);         // The port back to the PC, if connected
  delay(1000);
  LoadFromEprom();
  thingspeak.ConnectWifi(ssid, pass);  
  digitalWrite(LED_BUILTIN, LOW);
  //printWifiStatus();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  float therm = getThermister();
  thingspeak.Data(0, therm);  

  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.println("Read DHT11 failed");
    return;
  }
  thingspeak.Data(1, temperature);
  thingspeak.Data(2, humidity);
  
  int lightLevel  = analogRead(lightPin);
  thingspeak.Data(3, lightLevel);
  thingspeak.Send();

  digitalWrite(LED_BUILTIN, LOW);
  delay(30000); 
}


//void printWifiStatus()
//{
//  // print the SSID of the network you're attached to
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());
//
//  // print your WiFi shield's IP address
//  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);
//
//  // print the received signal strength
//  long rssi = WiFi.RSSI();
//  Serial.print("Signal strength (RSSI):");
//  Serial.print(rssi);
//  Serial.println(" dBm");
//  Serial.flush();
//}


float getThermister() {
  int tempReading = analogRead(tempPin);
  // This is OK
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  // float tempF = (tempC * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit

  return tempC;
}

void LoadFromEprom() {
  int ptr = 0;
  int value = EEPROM.read(ptr);
  ptr += sizeof(value);
  if (value == 255) {
    SelectNewSSID();
  }
  EEPROM.get(ptr, ssid);
  ptr += sizeof(ssid);
  EEPROM.get(ptr, pass);
  ptr += sizeof(pass);
  EEPROM.get(ptr, apikey);
}

void SelectNewSSID() {
  listNetworks();
  FlushSerialInput();
  Serial.setTimeout(10000);
  Serial.print("Please enter SSID:");
  Serial.readBytesUntil('\r', ssid, 15);
  int thisNet = atoi(ssid);
  strcpy(ssid, WiFi.SSID(thisNet));
  Serial.println();
  
  FlushSerialInput();
  Serial.print("Please enter Password:");
  Serial.readBytesUntil('\r', pass, 15);
  Serial.println();

  Serial.flush();

  int ptr = 0;
  int value = 1;
  EEPROM.put(ptr, value);
  ptr += sizeof(value);
  EEPROM.put(ptr, ssid);
  ptr += sizeof(ssid);
  EEPROM.put(ptr, pass);
  ptr += sizeof(pass);
  EEPROM.put(ptr, apikey);
}


void getFromSerial(char* rslt) {
  char buffer[20] = "";
  char c;
  int i = 0;
  do {
    if (Serial.available()) {
      c = Serial.read();
      if (((byte)c != 13) && ((byte)c != 10)) {
        buffer[i] = c;
        buffer[i + 1] = 0;
        i++;
      }
    }
  } while (c != '\r');
  for (int n = 0; n <= i; n++) {
    rslt[n] = buffer[n];
  }
  rslt[i + 1] = 0;
  FlushSerialInput();
}

void FlushSerialInput() {
  delay(100);
  while (Serial.available()) {
    Serial.read();
  }
}

void listNetworks()
{
  // scan for nearby networks
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }
  // print the list of networks seen
  Serial.print("Number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.println(" dBm");
  }
}



