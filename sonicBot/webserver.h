#pragma once
#ifndef webserver_h
	#define webserver_h
	//#######################   Includes
	#ifdef ESP32      //#####   ESP32
	#include <WiFi.h>
	#include <AsyncTCP.h>
	#include "SPIFFS.h"
	#elif defined(ESP8266)//#   ESP8266
	#include <ESP8266WiFi.h>
	#include <ESPAsyncTCP.h>
	#endif            // ####   ESP32/ESP8266
	#include <ArduinoJson.h>
	#include "ESPAsyncWebServer.h"//Webserver
	#include "FS.h"   //Include File System Headers

  extern bool manualMode;
	extern int angleX;
	extern int angleY;
	extern void setMotorSpeed(int leftMotor, int rightMotor);
	extern int angleToMotorSpeed (int angle);

extern "C" {
typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(void *);
}

struct btnSave {
  String id;
  callbackFunction function;
} ;

	// ### Webserver ###
	IPAddress apIP(192,1,1,1);
	AsyncWebServer server(80);
	AsyncWebSocket ws("/ws");
	const char* filename = "/index.html";
	const char* htmlRobotInUse = "/robotInUse.html";
	DynamicJsonDocument doc(2048);
  btnSave callbacks[6];
  int btnCounter = 0;
  AsyncWebSocketClient * wsclient;
  bool isConnect = false;

	bool initSPIFFS() {
  bool errorIndicator = false;
  //Initialize File System
  if (SPIFFS.begin())
  {
    #ifdef DEBUG
    Serial.println("SPIFFS Initialize....ok");
    #endif
  }
  else
  {
    #ifdef DEBUG
    Serial.println("SPIFFS Initialization...failed");
    #endif
    errorIndicator = false;
  }

  File f = SPIFFS.open(filename, "r");
  #ifdef DEBUG
  if (!f) {
    Serial.println("file open failed");
    errorIndicator = false;
  }
  else
  {
    //Write html site to file
    Serial.println("Writing Data to File");
  }
  #endif
  return errorIndicator;
}

void addCard(String name, String text, AsyncWebSocketClient * client){
  client->text("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"card\", \"action\":\"create\"}");
}

void changeCardText(String name, String text, AsyncWebSocketClient * client){
  client->text("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"card\", \"action\":\"update\"}");
}

void addButton(String name, String text, callbackFunction newFunction, AsyncWebSocketClient * client){
  client->text("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"btn\", \"action\":\"create\"}");
  callbacks[btnCounter].id = name;
  callbacks[btnCounter].function = newFunction;
  btnCounter++;
}

void changeButtonText(String name, String text, AsyncWebSocketClient * client){
  client->text("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"btn\", \"action\":\"update\"}");
}

// Websoket
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    wsclient = client;
    #ifdef DEBUG
      Serial.println("Websocket client connection received");
    #endif
    addButton("btn0", "User Button", [] {Serial.println("User action");}, client);
    addCard("sonnic", "Distance: ", client);
    addButton("btn1", "Change to sonnic mode", []() {
      manualMode = !manualMode;
      #ifdef DEBUG
         Serial.println("Manuelmode Changed");
      #endif
      if (manualMode) {
        changeButtonText("btn1", "Change to sonnic mode", wsclient);
      } else {
        changeButtonText("btn1", "Change to manuel mode", wsclient);
      }
      }, client);
  } else if(type == WS_EVT_DISCONNECT){
    wsclient = NULL;
    angleX = 90;
    angleY = 90;
    int leftMotor = angleToMotorSpeed(angleX);
    int rightMotor = leftMotor;
    setMotorSpeed(leftMotor, rightMotor);
    #ifdef DEBUG
      Serial.println("Client disconnected");
    #endif
  } else if(type == WS_EVT_DATA) {
    #ifdef DEBUG
      Serial.println("Got data");
    #endif
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      DeserializationError error = deserializeJson(doc, (char*)data);
      if (error) {
        #ifdef DEBUG
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
        #endif
        return;
      } else {
        if (doc.containsKey("button")) {
          String id = doc["button"];
          for (int i = 0; i<btnCounter; i++) {
            if (id.equals(callbacks[i].id)){
              callbacks[i].function();
            }
          }
        }
        if (doc.containsKey("angleX") && doc.containsKey("angleY")) {
          //TODO: change to function in Motorcontrol
          angleX = doc["angleX"];
          angleY = doc["angleY"];
          int leftMotor = angleToMotorSpeed(angleX);
          int rightMotor = leftMotor;
          double steer = angleToMotorSpeed(angleY) / 1023.0;
          if (steer < 0) {
            leftMotor = leftMotor + steer * leftMotor;
          } else {
            rightMotor = rightMotor - steer * rightMotor;
          }
          setMotorSpeed(leftMotor, rightMotor);
        }
      }
    }
  }
}

bool initWebserver(){
  bool initSucsess = true;
  #ifdef DEBUG
  Serial.println("SetAutoConnect");
  #endif
  WiFi.setAutoConnect(0);                    //Enables or Disaple WiFi Quick Connect

  //Initialize WiFi Settings
  #ifdef DEBUG
  Serial.println("SetAP");
  #endif
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  #ifdef DEBUG
  Serial.println("DefinesFehlen");
  #endif
  WiFi.softAP(BOT_NAME, BOT_PASSWORD); //Create WiFi hotspot
  #ifdef DEBUG
  Serial.println("Doch nicht");
  #endif
  // Popup DNS
  //dnsServer.start(DNS_PORT, "*", apIP);
  //server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  
  #ifdef DEBUG
  Serial.print("Access Point \"");
  Serial.println("\" started");
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
  #endif
  
  //Function to execute if user makes touch controls on mobile screen
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/index.html");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "favicon.ico", "image/x-icon");
  });

  // Websoket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.begin();
  return initSucsess;
}

#endif
