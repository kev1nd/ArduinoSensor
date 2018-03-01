# Arduino Sensor
An experimental project with an Arduino Uno linked to an ESP8266 ESP-01 Wifi module. 
It reads temperature, light levels and humidity, and sends the data to ThingSpeak.

The idea behind the project is learning and training, but it may become the basis of future IoT projects.

There are some good featues, that should be re-usable, for example.
1. Connect an Arduino to WiFi, and reliably send data using the WifiEsp library. (library also supports web service and more)
2. Interface to the ESP8266 ESP-01 module. This means a 3.3V voltage regulator for power (the Arduino 3.3V power can't provide enough current) and a TTL 5V to 3.3V step-down using two resistors.
3. Connect to the data agrogation service, thingspeak
4. Read and Write configuration data to Eeprom. The configuration remains even when the sketch is uploaded
5. Serial port used as data input form

https://thingspeak.com/channels/418675

## How it works
The Ardunino reads the sensors every 30 seconds and sends the data to https://thingspeak.com
https://thingspeak.com records the data, and displays it using some cool graphs.
A Wifi module is used to communicate with the network. This connects to Wifi, opens a TCP connection and sends an HTTP Get request containing the data fields. (Using GET to affectivey PUT data isn't great, but that seems to be how Thingspeak works)

### Electronics
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

![Circuit Diagram](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/circuitdiagram.jpg)

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


### Running for the first time
For the first time, run the Arduino connected to a serial port, and start the Serial Monitor (115200 Baud). Using the serial port as an entry-form, you will be asked if you want to configure - answer with "Y" to start the configuration process. If you do, you will be asked three questions:-

1. Wifi SSID
2. Wifi Password
3. Your Thingspeak API key (for writing to your channel)

If you either enter some data or wait for the timeout (10 seconds per question), the software will repeat the value entered and save the value into EEPROM. Of course, if you're too late and missed the question, just press the reset button on the Arduino.

If left alone after a reset, the values stored in EEPROM will be used.

The software should then sample the sensors once every 30 seconds, and update thingspeak.

## Future
1. A web service to configure, rather than a serial port form

<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/418675/charts/4?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
