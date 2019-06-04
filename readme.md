# GyroRobot

Use the ESP-12E WiFi module and the touch screen of your smartphone to control a robot


## Components used in this in this project:
- ESP-12E on NodeMCU 1.0
- H bridge L298N
- 2x DC motors
- HC-SR04 ultrasonic sensor

## Arduino IDE settings:
- Add ESP boards via Preferences / Settings and add the following url into the Additional Board Manager URLs:  http://arduino.esp8266.com/stable/package_esp8266com_index.json
- Add ESPAsyncWebServer library: https://github.com/me-no-dev/ESPAsyncWebServer
- Add ESPAsyncTCP library: https://github.com/me-no-dev/ESPAsyncTCP
- Install the Arduino ESP8266 filesystem uploader to make use of SPIFFS filesystem (https://github.com/esp8266/arduino-esp8266fs-plugin)
    - This plugin allows you to upload html files directly to the ESP
    - All files stored in the data subfolder of your Arduino sketch path will be uploaded   
- Board Settings:
    - Board: "NodeMCU 1.0 (ESP-12E Module)"
    - Flash Size: "4M (1M SPIFFS)"

## Wiring

### Wiring for L298N H bridge

L298N | ESP2866
------|-------------
enA   | D2  (GPIO4)
in1   | D3  (GPIO0)
in2   | D4  (GPIO2)
in3   | D0  (GPIO16)
in4   | D8  (GPIO15)
enB   | D1  (GPIO5)

### Wiring for HC-SR04 ultrasonic sensor

Attention: If the supply volatage of the ultrasonic sensor is 5V, you need to step down the 5V echo signal to 3.3V. 

HC-SR04 | ESP2866
--------|-------------
Trig    | D5  (GPI14)
Echo    | D6  (GPIO12) 

## Usage

- Upload the Arduino Sketch File with the recommended board settings.
- Upload the html file by cklicking on "Tools" --> "ESP8266 Sketch Data Upload"
- Connect to the Wifi network created by the ESP
- Open your webbrowser and go to 192.168.4.1
