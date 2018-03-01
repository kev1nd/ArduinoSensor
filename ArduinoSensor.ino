
// Arduino Data Collector
//
// Collects data and sends to ThingSpeak site for analysis
//
// Author: Kevin Dunwell
//
// Version 1.0.5
//
//
//  02/03/18  1.0.2   Changed SSID to VioletInternet for new hub
//  02/03/18  1.0.3   Add support for DHT11. Add support for Photo-resistor
//  02/03/18  1.0.4   Add WiFiEsp library and EEPROM storage for ssid/password
//  03/01/18  1.0.5   Remove WiFiEsp in favour of home-grown ESP8266/ThingSpeak library

#include <SimpleDHT.h>
#include <EEPROM.h>
#include <ThingSpeak8266.h>


// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;

ThingSpeak8266 thingspeak;
unsigned long waitTime = 30000; // Time to wait for a response
char ssid[15] = "";   // Wifi SSID
char pass[15] = "";   // Wifi Password
char apikey[16] = "";
int tempPin = 0;      // Sensor pin for Thermister
int lightPin = 1;     // Sensor pin for light level
int inputLevel = 2;
char fielddata[12] = "";

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);         // The port back to the PC, if connected
  LoadFromEprom();

  char ans[10];
  Serial.setTimeout(10000);
  Serial.print("Setup? Y/N:");
  Serial.readBytesUntil('\r', ans, 10);
  Serial.println(ans);

  if (ans[0]=='Y') {
    SelectNewSSID();
  }
  
  thingspeak.init(apikey);
  thingspeak.ConnectWifi(ssid, pass);

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

  float therm = getThermister();
  float volts = analogRead(inputLevel) / 18.65;
  
  dtostrf(therm, 4, 2, fielddata);
  thingspeak.Data(1, fielddata);
  dtostrf(temperature, 4, 2, fielddata);
  thingspeak.Data(2, fielddata);
  dtostrf(humidity, 4, 2, fielddata);
  thingspeak.Data(3, fielddata);
  dtostrf(lightLevel, 4, 2, fielddata);
  thingspeak.Data(4, fielddata);
  dtostrf(volts, 4, 2, fielddata);
  thingspeak.Data(5, fielddata);
  
  thingspeak.Send();

  digitalWrite(LED_BUILTIN, LOW);


  delay(30000);

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
  FlushSerialInput();
  Serial.setTimeout(10000);
  Serial.print("Please enter SSID (");
  Serial.print(ssid);
  Serial.print(") :");
  Serial.readBytesUntil('\r', ssid, 15);
  Serial.println(ssid);
  
  FlushSerialInput();
  Serial.print("Please enter Password:");
  Serial.readBytesUntil('\r', pass, 15);
  Serial.println(pass);

  FlushSerialInput();
  Serial.print("Please enter your ThingSpeak api key (");
  Serial.print(apikey);
  Serial.print(") :");
  Serial.readBytesUntil('\r', apikey, 16);
  Serial.println(apikey);

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


