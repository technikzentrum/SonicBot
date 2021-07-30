//#######################   Includes
#ifdef ESP32      //#####   ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#elif defined(ESP8266)//#   ESP8266
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif            // ####   ESP32/ESP8266
//#include <DNSServer.h>// Redirect
#include "ESPAsyncWebServer.h"//Webserver
#include "FS.h"   //Include File System Headers
#include <ArduinoOTA.h>// Wlan Updates
#include <ArduinoJson.h>


//#######################   GLOBAL Varriables

// ### User Variables
#define botName "myBot" // <---- change me
#define botPassword "" // <---- change me
#define DEBUG           // Uncomment for non Serial Use

// ### DNS Varriables ###
//const byte DNS_PORT = 53;
IPAddress apIP(192,1,1,1);
//DNSServer dnsServer;

// ### Sonnic Varriables ###
long duration = 0;
long distance = 0;

// ### Heartbeat ###
bool deviceIsConnected = false;

// ### Webserver ###
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const char* filename = "/index.html";
const char* htmlRobotInUse = "/robotInUse.html";
DynamicJsonDocument doc(2048);

// ### Bot Status ###
int angleX = 90;
int angleY = 90;
bool manualMode = true;

//#######################   PIN Defines
#ifdef ESP32
// ### Defines for H bridge ###
#define enA 23 //D2
#define enALEDChannel 0
#define in1 22 //D3   
#define in2 21 //D4   
#define in3 19 //D0
#define in4 18 //S3
#define enB 5 //D1
#define enBLEDChannel 1
#define DEAD_BAND 200
// ### Defines for Sonic Module ###
#define TRIGGER 17
#define ECHO 16
#elif defined(ESP8266)
// ### Defines for H bridge ###
#define enA 4 //D2
#define in1 0 //D3   
#define in2 2 //D4   
#define in3 16 //D0
#define in4 D8 //S3
#define enB 5 //D1
#define DEAD_BAND 200
// ### Defines for Sonic Module ###
#define TRIGGER D5
#define ECHO D6
#endif

//#######################   Defines for Arduino OTA
//#define OTAUpdate //uncomment line to enable OTA update



//#######################   Functions Motor
void setMotorSpeed(int leftMotor, int rightMotor) {
  if (leftMotor < - DEAD_BAND) {   //Turn left
    #ifdef ESP32
    ledcWrite(enALEDChannel, - leftMotor);
    #elif defined(ESP8266)
    analogWrite(enA, - leftMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    #ifdef DEBUG
    Serial.println("Left motor: " + String((leftMotor*-1)));
    #endif
  } else if (leftMotor > DEAD_BAND) {
    #ifdef ESP32
    ledcWrite(enALEDChannel, leftMotor);
    #elif defined(ESP8266)
    analogWrite(enA, leftMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    #ifdef DEBUG
    Serial.println("Left motor: " + String(leftMotor));
    #endif
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    #ifdef DEBUG
    Serial.println("Left motor: STOPPED");
    #endif
  }

  if (rightMotor < - DEAD_BAND) {   //Turn left
    #ifdef ESP32
    ledcWrite(enBLEDChannel, - rightMotor);
    #elif defined(ESP8266)
    analogWrite(enB, - rightMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    #ifdef DEBUG
    Serial.println("Right motor: " + String((rightMotor*-1)));
    #endif
  } else if (rightMotor > DEAD_BAND) {
    #ifdef ESP32
    ledcWrite(enBLEDChannel, rightMotor);
    #elif defined(ESP8266)
    analogWrite(enB, rightMotor); // Send PWM signal to L298N Enable pin
    #endif
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    #ifdef DEBUG
    Serial.println("Right motor: " + String(rightMotor));
    #endif
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    #ifdef DEBUG
    Serial.println("Right motor: STOPED");
    #endif
  }
}


//#######################   Functions Webpage
/*
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};
*/
/**
   Function to convert the touch inputs from the Website to a motor speed.
*/
int angleToMotorSpeed (int angle) {
  long motorSpeed = 0;
  motorSpeed = angle * (1023 / 45) - 2048;
  if (motorSpeed < -1023) {
    motorSpeed = -1023;
  }
  if (motorSpeed > 1023) {
    motorSpeed = 1023;
  }
  return motorSpeed;
}

// Websoket
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    deviceIsConnected = true;
    Serial.println("Websocket client connection received");
    client->text("Hello from ESP32 Server");
  } else if(type == WS_EVT_DISCONNECT){
    deviceIsConnected = false;
    angleX = 90;
    angleY = 90;
    int leftMotor = angleToMotorSpeed(angleX);
    int rightMotor = leftMotor;
    setMotorSpeed(leftMotor, rightMotor);
    Serial.println("Client disconnected");
  } else if(type == WS_EVT_DATA) {
    Serial.println("Got data");
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      DeserializationError error = deserializeJson(doc, (char*)data);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      } else {
        if (doc.containsKey("button")) {
          long time = doc["button"];
        }
        if (doc.containsKey("angleX") && doc.containsKey("angleY")) {
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

//#######################   Functions Sonnic
/**
   Function to receive the distance measured by the us-sensor in cm
*/
long getUSDistance() {
  digitalWrite(TRIGGER, LOW); //Hier nimmt man die Spannung für kurze Zeit vom Trigger-Pin, damit man später beim senden des Trigger-Signals ein rauschfreies Signal hat.
  delay(5); //duration: 5 Millisekunden
  digitalWrite(TRIGGER, HIGH); //Jetzt sendet man eine Ultraschallwelle los.
  delay(10); //Dieser „Ton“ erklingt für 10 Millisekunden.
  digitalWrite(TRIGGER, LOW);//Dann wird der „Ton“ abgeschaltet.
  duration = pulseIn(ECHO, HIGH); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.
  distance = (duration / 2) * 0.03432; //Nun berechnet man die distance in Zentimetern. Man teilt zunächst die Zeit durch zwei (Weil man ja nur eine Strecke berechnen möchte und nicht die Strecke hin- und zurück). Den Wert multipliziert man mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.
  return distance;
}

//#######################   SETUP
void setup()
{
  //Serial.begin (115200);
  #ifdef DEBUG
  Serial.begin (115200); //Start serial communication
  Serial.println(doc.capacity());
  Serial.println("Setup Begin\nPindefines");
  #endif
  //Initialize H bridge
  #ifdef ESP32
  ledcAttachPin(enA, enALEDChannel);
  ledcAttachPin(enB, enBLEDChannel);
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(enALEDChannel, 4000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(enBLEDChannel, 4000, 8); // 12 kHz PWM, 8-bit resolution
  #elif defined(ESP8266)
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  #endif
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  //Init US
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);



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
  }

  File f = SPIFFS.open(filename, "r");
  #ifdef DEBUG
  if (!f) {
    Serial.println("file open failed");
  }
  else
  {
    //Write html site to file
    Serial.println("Writing Data to File");
  }
  #endif
  WiFi.setAutoConnect(0);                    //Enables or Disaple WiFi Quick Connect

  //Initialize WiFi Settings
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(botName, botPassword); //Create WiFi hotspot
  
  // Popup DNS
  //dnsServer.start(DNS_PORT, "*", apIP);
  //server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  
  #ifdef DEBUG
  Serial.print("Access Point \"");
  Serial.println("\" started");
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
  #endif
  //Arduino OTA
#ifdef OTAUpdate
  ArduinoOTA.setHostname("BOTOTA");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    #ifdef DEBUG
    Serial.println("Start updating " + type);
    #endif
  });
  ArduinoOTA.onEnd([]() {
    #ifdef DEBUG
    Serial.println("\nEnd");
    #endif
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    #ifdef DEBUG
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    #endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
    #ifdef DEBUG
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
    #endif
  });
  ArduinoOTA.begin();
#endif


  /**
     Main server part starts here.
  */

  //Function to execute if user makes touch controls on mobile screen
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      //deviceIsConnected = true;
      request->send(SPIFFS, "/index.html");
  });
  // Websoket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  server.begin();

  #ifdef DEBUG
  Serial.println("Setup Finished");
  #endif
}


void loop()
{
  if (manualMode) {
    yield();
  } else {
    /**
       TODO: Implement code for us-sensor
       The following functions might be helpful:
           long distance = getUSDistance();         //Get us-distance in cm
           Serial.println(distance);                //print distance to console
           setMotorSpeed(1023, 1023);               //set motor speed for left and right wheel
           delay(100)                               //Add additional delay
    */
  }
#ifdef OTAUpdate
  ArduinoOTA.handle();
#endif
//dnsServer.processNextRequest();
}
