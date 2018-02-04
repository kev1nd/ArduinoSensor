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
String ssid = "VioletInternet"; // Wifi SSID
String wifipw = "kjjj1997";     // Wifi Password
int tempPin = 0;                // Sensor pin for Thermister
int lightPin =1;                // Sensor pin for light level

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);         // The port back to the PC, if connected
  Serial1.begin(9600);          // ESP8266 serial port
  connectwifi();
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



//  if (Serial.available())
//  {
//    // wait to let all the input command in the serial buffer
//    delay(10);
//
//    // read the input command in a string
//    String cmd = "";
//    while (Serial.available())
//    {
//      cmd += (char)Serial.read();
//    }
//
//    // print the command and send it to the ESP
//    Serial.println();
//    Serial.print(">>>> ");
//    Serial.println(cmd);
//
//    Serial1.print(cmd);
//  }

  delay(30000);

}


bool connectwifi() {
  bool success = true;
  int stage = 3;
  while (success && (stage > 0)) {
    switch (stage) {
      case 3:
        success = SendCommand("AT+CWMODE=1");
        break;
      case 2:
        success = SendCommand("AT+CWQAP");
        break;
      case 1:
        success = SendCommand("AT+CWJAP=\"" + ssid + "\",\"" + wifipw + "\"");
        break;
      default:
        stage = 0;
        break;
    }
    stage--;
  }
  return success;
}


bool connectTCP() {
  bool success = true;
  int stage = 2;
  while (success && (stage > 0)) {
    switch (stage) {
      case 2:
        success = SendCommand("AT+CIPMUX=1");
        break;
      case 1:
        success = SendCommand("AT+CIPSTART=4,\"TCP\",\"52.5.13.84\",80", "ALREADY CONNECT");
        break;
      default:
        stage = 0;
        break;
    }
    stage--;
  }
  return success;
}





bool sendData(String field1, String field2, String field3, String field4) {
  String dataStr = "GET https://api.thingspeak.com/update?api_key=WZDTWH6PEE2G0YII&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4;
  bool success = true;
  connectTCP();
  int stage = 2;
  while (success && (stage > 0)) {
    switch (stage) {
      case 2:
        success = SendCommand("AT+CIPSEND=4," + String(dataStr.length() + 2));
        break;
      case 1:
        success = SendCommand(dataStr);
        break;
      default:
        stage = 0;
        break;
    }
    stage--;
  }
  return success;
}


bool SendCommand(String cmd) {
  SendCommand(cmd, waitTime);
}

bool SendCommand(String cmd, int waitFor) {
  String rsp = "";
  SendCommand(cmd, waitFor, "OK\r\n", rsp);
}

bool SendCommand(String cmd, String expectedResp) {
  String rsp = "";
  SendCommand(cmd, waitTime, expectedResp, rsp);
}

bool SendCommand(String cmd, int waitFor, String expectedResp, String &actualResp) {
  bool rslt = false;
  bool done = false;
  Serial.println(">>>" + cmd);
  Serial.flush();
  Serial1.println(cmd);
  Serial1.flush();

  unsigned long stopMillis = millis() + waitFor;
  String rsp = "";

  while ((!done) && (millis() < stopMillis)) {
    delay(100);
    if (Serial1.available())
    {
      delay(100);
      while (Serial1.available())
      {
        rsp += (char)Serial1.read();
      }
      Serial.print("<<<" + cmd);
      Serial.println(rsp);
      Serial.flush();
      if ((rsp.indexOf("OK") >= 0) || (rsp.indexOf(expectedResp) >= 0)) { // Maybe try "OK\r\n" as well
        rslt = true;
        done = true;
      }
      if (rsp.indexOf("ERROR") >= 0) { // Maybe try "ERROR\r\n" as well
        rslt = false;
        done = true;
      }
    }
  }
  actualResp = rsp;
  if (rslt) {
    Serial.println("(SUCCESS)");
  } else if (done) {
    Serial.println("(FAIL)");
  } else {
    Serial.println("(TIMEOUT)");
  }
  Serial.flush();
  return rslt;
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

