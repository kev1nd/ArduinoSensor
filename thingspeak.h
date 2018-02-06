#ifndef thingspeak_h
#define thingspeak_h

#include <inttypes.h>
#include "Print.h"
#include <WiFiEsp.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
#endif


class ThingSpeak {
  public:
    ThingSpeak::ThingSpeak(char * key);
    void ThingSpeak::ConnectWifi(char *ssid, char *password);
    int ThingSpeak::Status();
    void ThingSpeak::Data(int ref, float fielddata);
    void ThingSpeak::Send();

    
  private:
    float data[8];
    bool hasdata[8];
    char apikey[17];
    WiFiEspClient client;
    int status;
};

#endif

