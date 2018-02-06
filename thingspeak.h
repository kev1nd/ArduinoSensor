#ifndef thingspeak_h
#define thingspeak_h

#include <inttypes.h>
#include "Print.h"
#include <WiFiEsp.h>



class ThingSpeak {
  public:
    ThingSpeak::ThingSpeak(char * key, Stream *serialPort);
    void ThingSpeak::ConnectWifi(char *ssid, char *password);
    void ThingSpeak::Data(int ref, float fielddata);
    void ThingSpeak::Send();

    
  private:
    float data[8];
    bool hasdata[8];
    char apikey[20];
    Stream *Port;
    WiFiEspClient client;
    int status;
};

#endif

