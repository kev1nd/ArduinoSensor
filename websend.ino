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

#include <WiFiEsp.h>
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
int status = WL_IDLE_STATUS;    // the Wifi radio's status
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

  LoadFromEprom();


  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  int tryCount = 0;
  while ( status != WL_CONNECTED) {
    if ((tryCount > 3) || (strlen(ssid) == 0)) {
      tryCount = 0;
      SelectNewSSID();
    }
    tryCount++;
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  digitalWrite(LED_BUILTIN, LOW);
  printWifiStatus();

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

  float therm = getThermister();
  
  sendDataToThingSpeak(therm, temperature, humidity, lightLevel);

  digitalWrite(LED_BUILTIN, LOW);

  delay(30000);

  if (client.available()) {
    String resp = "";
    while (client.available())
    {
      resp += (char)client.read();
    }
    Serial.println("Remote response: " + resp);
  }
}



void sendDataToThingSpeak(float field1, int field2, int field3, int field4) {
  int cCount = 0;

  char field1str[10];
  dtostrf(field1, 4, 2, field1str);

  char myCommand[120];
  sprintf(myCommand, "GET http://api.thingspeak.com/update?api_key=%s&field1=%s&field2=%i&field3=%i&field4=%i", apikey, field1str, field2, field3, field4);
  Serial.println(myCommand);

  // Could try "AT+CIPSEND=4," + String(myCommand.length() + 2));

  client.stop();
  if (client.connect(server, 80)) {      // connectSSL and port 443 also work
    cCount = client.println(myCommand);  // TIMEOUT comes here, so no response is received  :-(
    client.println();
  }
  else {
    Serial.println("Connection failed");
  }
//  Serial.print("Characters written = ");
//  Serial.println(cCount);

  delay(1000);
  if (client.available()) {
    char* resp = "";
    while (client.available())
    {
      resp += (char)client.read();
    }
    Serial.print("Remote response:");
    Serial.println(resp);
  }

}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.flush();
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



