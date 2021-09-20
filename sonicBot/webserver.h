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
// Globals
#ifdef ESP32// no Racecondition
static SemaphoreHandle_t mutex;
#endif
  extern bool manualMode;
	extern int angleX;
	extern int angleY;
	extern void setMotorSpeed(int leftMotor, int rightMotor);
	extern int angleToMotorSpeed (int angle);
 extern ConfigLoad configSet;

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
	DynamicJsonDocument doc(4096);
  DynamicJsonDocument docSend(4096);
  JsonArray dataSend;
  btnSave callbacks[6];
  int btnCounter = 0;
  AsyncWebSocketClient * wsclient;
  AsyncEventSource events("/events");

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

void webElement(String name, String text, String typee, String action) {
  #ifdef ESP32// no Racecondition
  xSemaphoreTake(mutex, portMAX_DELAY);
  #endif
  int fund = 0;
  for (JsonVariant value : dataSend) {
    String ss = value;
    if (ss.indexOf((String("\"name\":\"") + name)) != -1) {
      if ( ss.indexOf("\"action\":\"" + action + "\"") != -1){
        value.set(serialized("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"" + typee + "\", \"action\":\"" + action + "\"}"));
        fund = 1;
        //Serial.println("ersetze");
        break;
      }
    }
  }
  if (!fund) {
    dataSend.add(serialized("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"" + typee + "\", \"action\":\"" + action + "\"}"));
  }
  #ifdef ESP32// no Racecondition
  xSemaphoreGive(mutex);
  #endif
}

void changeCardText(String name, String text){
  webElement(name, text, "card", "update");
}

void addButton(String name, String text, callbackFunction newFunction, AsyncWebSocketClient * client){
  client->text("{\"name\":\"" + name + "\", \"text\":\"" + text + "\", \"type\":\"btn\", \"action\":\"create\"}");
  callbacks[btnCounter].id = name;
  callbacks[btnCounter].function = newFunction;
  btnCounter++;
}

void changeButtonText(String name, String text){
  webElement(name, text, "btn", "update");
}

void updateWebPage() {
  #ifdef ESP32// no Racecondition
  xSemaphoreTake(mutex, portMAX_DELAY);
  #endif
    char output[512];
    serializeJson(docSend, output);
    #ifdef DEBUG
      Serial.println(output);
    #endif
    docSend.clear();
    dataSend = docSend.createNestedArray("data");
  #ifdef ESP32// no Racecondition
  xSemaphoreGive(mutex);
  #endif
  #ifdef DEBUG
      Serial.println(strlen(output));
  #endif
  if (strlen(output)>12) {
    events.send(output);
  }
}

// Websoket
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    if (ws.getClients().length()>1) {
      client->text("{\"type\":\"disconnect\"}");
      return;
    }
    #ifdef DEBUG
      Serial.println("Websocket client connection received");
    #endif
    addButton("btn0", "User Button", [] {
      #ifdef DEBUG
        Serial.println("User action");
      #endif
      }, client);
    addCard("left", "MotorL:0 % ", client);
    addCard("sonnic", "Distance: ", client);
    addCard("right", "MotorR:0 % ", client);
    wsclient = client;
    addButton("btn1", "Change to sonnic mode", []() {
      manualMode = !manualMode;
      #ifdef DEBUG
         Serial.println("Manuelmode Changed");
      #endif
      if (manualMode) {
        changeButtonText("btn1", "Change to sonnic mode");
      } else {
        changeButtonText("btn1", "Change to manuel mode");
      }
      }, client);
  } else if(type == WS_EVT_DISCONNECT){
    if (ws.getClients().length()==0) {
      btnCounter = 0;
    }
    btnCounter = 0;
    wsclient = NULL;
    angleX = 90;
    angleY = 90;
    int leftMotor = angleToMotorSpeed(angleX);
    int rightMotor = leftMotor;
    changeCardText("left", "MotorL: 0 % ");
    changeCardText("right", "MotorR: 0 % ");
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
        doc.clear();
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
          #ifdef DEBUG
            Serial.print("ax: ");
            Serial.println(angleX);
            Serial.print("aY: ");
            Serial.println(angleY);
            Serial.print("l: ");
            Serial.println(leftMotor);
            Serial.print("r: ");
            Serial.println(rightMotor);
            Serial.print("Steer: ");
            Serial.println(steer);
          #endif
          if (steer < 0) {
            leftMotor = leftMotor + steer * leftMotor;
          } else {
            rightMotor = rightMotor - steer * rightMotor;
          }
          
          
          changeCardText("left", "MotorL: " + String(map(leftMotor, 0, 1024, 0, 100) ) + " % ");
          changeCardText("right", "MotorR: " + String(map(rightMotor, 0, 1024, 0, 100)) + " % ");
          setMotorSpeed(leftMotor, rightMotor);
        }
      }
      doc.clear();
    }
  }
}

String processor(const String& var)
{
  if(var == "BOT_NAME") {
    return configSet.ssid;
  } else if (var == "DEAD_BAND") {
    return String(configSet.deadBand);
  } else if (var == "BOT_PASSWORD") {
    return String(configSet.pw);
  }
  Serial.println(var);
  return String();
}

bool initWebserver(){
  dataSend = docSend.createNestedArray("data");
  #ifdef ESP32// no Racecondition
  mutex = xSemaphoreCreateMutex();
  #endif
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
  #ifdef ESP32
    WiFi.softAP(configSet.ssid, configSet.pw); //Create WiFi hotspot
  #else
    WiFi.softAP(configSet.ssid, configSet.pw); //Create WiFi hotspot
  #endif
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

//.setTemplateProcessor(processor)
  server.serveStatic("/", SPIFFS, "/").setTemplateProcessor(processor).setDefaultFile("index.html");
  /*//Function to execute if user makes touch controls on mobile screen
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/index.html");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "favicon.ico", "image/x-icon");
  });*/
  server.on("/dcAll", HTTP_GET, [](AsyncWebServerRequest * request) {
      ws.textAll("{\"type\":\"disconnect\"}");
      ws.closeAll();
      if (ws.getClients().length()==0){
        request->redirect("/index.html");
      }else {
        request->redirect("/dcAll");
      }
  });
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      int paramsNr = request->params();
      for (int i = 0; i < paramsNr; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->name() == "name") {
          p->value().toCharArray(configSet.ssid, 25);
        } else if (p->name() == "pw") {
          p->value().toCharArray(configSet.pw, 25);
        } else if (p->name() == "deadBand") {
          configSet.deadBand = p->value().toInt();
        } else {
          #ifdef DEBUG
            Serial.print("unknown name: ");
            Serial.print(p->name());
            Serial.print(", value to int: ");
            Serial.println(p->value().toInt());
          #endif
        }
      }
      if (request -> params() == 0) {
        request->send(SPIFFS, "/index.html");
      } else {
        saveInEEPROM();
        request -> send(200);
      }
  });
  // Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      #ifdef DEBUG
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
      #endif
    }
  });
  server.addHandler(&events);
  // Websoket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.begin();
  return initSucsess;
}

#endif
