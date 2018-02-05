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

![Another picture of prototype board](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/pic2.jpg)

## Building your version
This is the wiring diagram for the project.
It's not complete yet (needs light-sensitice resistor circuit).

![Circuit Diagram](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/circuit.JPG)

### Configure your ESP8266 for 9600 baud
The ESP8622 unit I bought was set to run at 115200 baud, which is too fast for software serial emulation on the Arduino to keep up with. However, sending this AT command to the unit will permanently configure it to 9600 baud:

`AT+UART_DEF=9600,8,1,0,0`

There are several articles around with sketches to send comms to the ESP8266 through Serial1. Since the Arduino will send, but not receive, quite happily at 115200, these can be used with this circuit. Alternatively, you can use an ESP8266 USB adapter talk to it directly from a PC.

### Change the ThingSpeak Key
Create yourself an account at https://thingspeak.com and create a channel. You will need four fields:-
1. Temperature (Thermister)
2. Temperature (DHT11)
3. Humidity (DHT11)
4. Light Level

Change the thingspeak key (top of code) to your channel key.

### Running for the first time
Once you compile and run the code for the first time, it will try (and fail) to connect to Wifi. It will then list the found Wifi servers and ask you to select one using the serial port. The default baud rate for this is 115200. Enter the *number* next to the list to select one, then answer the password question.

The software should then sample the sensors once every 30 seconds, and update thingspeak.

## Future
1. I plan to make the thingspeak key a configurable item, like the ssid and password
2. There is a fault with the current version of the WifiEsp library, causing timeout reports. I plan to download and rebuild this library.
3. A means of changing the wifi connection is needed - currently, this can only happen if the wifi connection can't happen
4. A web service to configure, rather than a serial port form

