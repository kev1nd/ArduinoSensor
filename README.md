# Arduino Sensor
An experimental project with an Arduino Uno linked to an ESP8266 ESP-01 Wifi module. 
It reads temperature, light levels and humidity, and sends the data to ThingSpeak.


The idea behind the project is learning and training, but it may become the basis of future IoT projects.

## How it works
The Ardunino reads the sensors every 30 seconds and sends the data to https://thingspeak.com
https://thingspeak.com records the data, and displays it using some cool graphs
Other features in the code include:
1. Use of the WifiEsp library to connect to Wifi using an ESP8266
2. Stores the SSID and password in Eeprom
3. If Wifi connection fails, it asks for the SSID and password through the serial monitor
4. Convertion of thermister voltage to Kelvin and Celcius readings


## Electronics
The electronics consists of:
1. An Arduino Uno, mounted next to a breadboard
2. An ESP8266 ESP-01, with standard AT Eprom
3. A 3.3V regulator, and step-down TTL voltage converter, to protect the ESP8266
4. A Thermister
5. A DHT11 temperature and humidity sensor
6. A light-sensitive resistor

![Picture of prototype board](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/pic1.jpg)

![Picture of prototype board](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/pic2.jpg)

## Building your version
I will produce a circuit diagram in due course.

Download and compile the code using the Arduino IDE
