#include "thingspeak.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"


ThingSpeak::ThingSpeak(char * key, Stream *serialPort)
{
  strcpy(apikey, key);
  Port = serialPort;
  status = WL_IDLE_STATUS;
  for (int i = 0; i < 8; i++) {
    data[i] = 0;
    hasdata[i] = false;
  }
}

void ThingSpeak::ConnectWifi(char *ssid, char *password) {
  WiFi.init(Port);
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
    }
    tryCount++;
    status = WiFi.begin(ssid, password);
  }
}

void ThingSpeak::Data(int ref, float fielddata) {
  data[ref] = fielddata;
  hasdata[ref] = true;
}


void ThingSpeak::Send() {
  int cCount = 0;

  char fieldstr[10];
  char myCommand[200];
  sprintf(myCommand, "GET http://api.thingspeak.com/update?api_key=%s", apikey);
  for (int i = 0; i < 8; i++) {
    if (hasdata[i]) {
      dtostrf(data[i], 4, 2, fieldstr);
      sprintf(myCommand, "%s&field%i=%s", myCommand, i, fieldstr);
      data[i] = 0;
      hasdata[i] = false;
    }
  }
  Serial.println(myCommand);

  // Could try "AT+CIPSEND=4," + String(myCommand.length() + 2));

  client.stop();
  if (client.connect("api.thingspeak.com", 80)) {      // connectSSL and port 443 also work
    cCount = client.println(myCommand);  // TIMEOUT comes here, so no response is received  :-(
    client.println();
  }
  else {
    Serial.println("Connection failed");
  }
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

