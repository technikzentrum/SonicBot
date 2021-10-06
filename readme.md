# SonicBot

Use the ESP8266 OR ESP32 WiFi module and the touch screen of your smartphone to control a robot


## Components used in this in this project:
- ESP-12F (NodeMCU Lua Lolin V3 Module ESP8266 ESP-12F) | ESP32
- H bridge L298N
- 2x DC motors
- HC-SR04 ultrasonic sensor

## Arduino IDE settings:
- Add ESP boards via Preferences / Settings and add the following url into the Additional Board Manager URLs:
ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json
OR
ESP32: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json


- Add ESPAsyncWebServer library:
ESP8266 AND ESP32: https://github.com/me-no-dev/ESPAsyncWebServer

- Add ESPAsyncTCP library:
ESP8266: https://github.com/me-no-dev/ESPAsyncTCP
OR
ESP32: https://github.com/me-no-dev/AsyncTCP

- Install the Arduino ESP filesystem uploader to make use of SPIFFS filesystem
ESP8266: https://github.com/esp8266/arduino-esp8266fs-plugin
ESP32: https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/
    - This plugin allows you to upload html files directly to the ESP
    - All files stored in the data subfolder of your Arduino sketch path will be uploaded   
- Board Settings:
ESP8266:
    - Board: "NodeMCU 1.0 (ESP-12E Module)"
    - Flash Size: "4M (1M SPIFFS)"
ESP32: your bord or DEV module

## Wiring

### Wiring for L298N H bridge

L298N | ESP8266
------|-------------
enA   | D2  (GPIO4)
in1   | D3  (GPIO0)
in2   | D4  (GPIO2)
in3   | D0  (GPIO16)
in4   | D8  (GPIO15)
enB   | D1  (GPIO5)

### Wiring for HC-SR04 ultrasonic sensor

Attention: If the supply volatage of the ultrasonic sensor is 5V, you need to step down the 5V echo signal to 3.3V. 

HC-SR04 | ESP8266
--------|-------------
Trig    | D5  (GPI14)
Echo    | D6  (GPIO12) 

## Usage

- Upload the Arduino Sketch File with the recommended board settings.
- Upload the html file by cklicking on "Tools" --> "ESP(8266/32) Sketch Data Upload"
- Connect to the Wifi network created by the ESP
- Open your webbrowser and navigate to 192.1.1.1
- On Android devices: You may have to disable mobile data usage in order to connect to the local IP-Address
