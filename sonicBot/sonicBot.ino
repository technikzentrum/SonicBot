//#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"   //Include File System Headers
#include "SSD1306Spi.h"
#include <ESP8266HTTPClient.h>
#include <Ticker.h>  //Ticker Library for timer Interrupt

//Defines for H bridge
#define enA 4 //D2
#define in1 0 //D3   
#define in2 2 //D4   
#define in3 16 //D0
#define in4 D8 //S3
#define enB 5 //D1
#define DEAD_BAND 200

//Defines for US
#define debugLED 13 //D
int trigger = D5; //Trigger-Pin US
int echo = D6; // Echo-Pin US
long dauer = 0; // Das Wort dauer ist jetzt eine Variable, unter der die Zeit gespeichert wird, die eine Schallwelle bis zur Reflektion und zurück benötigt. Startwert ist hier 0.
long entfernung = 0; // Das Wort „entfernung“ ist jetzt die variable, unter der die berechnete Entfernung gespeichert wird. Info: Anstelle von „int“ steht hier vor den beiden Variablen „long“. Das hat den Vorteil, dass eine größere Zahl gespeichert werden kann. Nachteil: Die Variable benötigt mehr Platz im Speicher.


Ticker checkIPAddress;


const char* filename = "/index.html";
const char* htmlRobotInUse = "/robotInUse.html";
int angleX = 90;
int angleY = 90;
bool deviceIsConnected = false;
bool manualMode = false;
String currentClientIPAddress;


AsyncWebServer server(80);

void setMotorSpeed(int leftMotor, int rightMotor) {
  if (leftMotor < - DEAD_BAND) {   //Turn left
    analogWrite(enA, - leftMotor); // Send PWM signal to L298N Enable pin
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    //Serial.println(leftMotor);
  } else if (leftMotor > DEAD_BAND) {
    analogWrite(enA, leftMotor); // Send PWM signal to L298N Enable pin
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }

  if (rightMotor < - DEAD_BAND) {   //Turn left
    analogWrite(enB, - rightMotor); // Send PWM signal to L298N Enable pin
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    //Serial.print(enB);
    //Serial.println(" Right");
  } else if (rightMotor > DEAD_BAND) {
    analogWrite(enB, rightMotor); // Send PWM signal to L298N Enable pin
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    //Serial.print(enB);
    //Serial.println(" Left");
  } else {
    //Serial.println("STOP");
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}

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

void checkForIPAddress() {
  if (currentClientIPAddress != "") {
    Serial.println("Reset IP Address");
    deviceIsConnected = false;
    currentClientIPAddress = "";
    angleX = 90;
    angleY = 90;
    int leftMotor = angleToMotorSpeed(angleX);
    int rightMotor = leftMotor;
    setMotorSpeed(leftMotor, rightMotor);
  }
}

void moveWheels() {
  for (int i = 0; i < 4; i++) {
    setMotorSpeed(1023, 1023);
    delay(100);
    setMotorSpeed(0, 0);
    delay(100);
  }
}

long getUSDistance() {
  digitalWrite(trigger, LOW); //Hier nimmt man die Spannung für kurze Zeit vom Trigger-Pin, damit man später beim senden des Trigger-Signals ein rauschfreies Signal hat.
  delay(5); //Dauer: 5 Millisekunden
  digitalWrite(trigger, HIGH); //Jetzt sendet man eine Ultraschallwelle los.
  delay(10); //Dieser „Ton“ erklingt für 10 Millisekunden.
  digitalWrite(trigger, LOW);//Dann wird der „Ton“ abgeschaltet.
  dauer = pulseIn(echo, HIGH); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.
  entfernung = (dauer / 2) * 0.03432; //Nun berechnet man die Entfernung in Zentimetern. Man teilt zunächst die Zeit durch zwei (Weil man ja nur eine Strecke berechnen möchte und nicht die Strecke hin- und zurück). Den Wert multipliziert man mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.
  return entfernung;
}

void setup()
{
  Serial.begin (115200); //Serielle kommunikation starten, damit man sich später die Werte am serial monitor ansehen kann.
  Serial.println("Setup Begin");
  //Initialize H bridge
  pinMode(debugLED, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
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
  pinMode(trigger, OUTPUT); // Trigger-Pin ist ein Ausgang
  pinMode(echo, INPUT); // Echo-Pin ist ein Eingang

  //Initialize File System
  if (SPIFFS.begin())
  {
    Serial.println("SPIFFS Initialize....ok");
  }
  else
  {
    Serial.println("SPIFFS Initialization...failed");
  }

  File f = SPIFFS.open(filename, "r");

  if (!f) {
    Serial.println("file open failed");
  }
  else
  {
    //Write html site to file
    Serial.println("Writing Data to File");
  }

  WiFi.setAutoConnect(0);                    //Enables or Disaple WiFi Quick Connect

  //Initialize WiFi Settings
  WiFi.softAP("SchulRobotAP", ""); //Create WiFi hotspot
  WiFi.mode(WIFI_AP);
  Serial.print("Access Point \"");
  Serial.println("\" started");
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer




  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (currentClientIPAddress == "" || currentClientIPAddress == request->client()->remoteIP().toString()) {
      currentClientIPAddress = request->client()->remoteIP().toString();
      checkIPAddress.detach(); //Disable Timer
      deviceIsConnected = true;
      int paramsNr = request->params();
      //Serial.println(paramsNr);
      for (int i = 0; i < paramsNr; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->name() == "angleX") {
          angleX = p->value().toInt();
          //   Serial.print("Angle x:");
          //   Serial.println(angleX);
        } else if (p->name() == "angleY") {
          angleY = p->value().toInt();
          //    Serial.print("Angle y:");
          //    Serial.println(angleY);
        } else {
          Serial.print("unknown name: ");
          Serial.print(p->name());
          Serial.print(", value to int: ");
          Serial.println(p->value().toInt());
        }
      }
      if (request -> params() == 0) {
        request->send(SPIFFS, "/index.html");
      } else {
        request -> send(200);
      }
      checkIPAddress.attach(2, checkForIPAddress); //Enable Timer again
    } else {
      request->send(SPIFFS, "/robotInUse.html");
      Serial.print("Declined Connection from: ");
      Serial.print(request->client()->remoteIP().toString());
      Serial.print(", The following device is already connected: ");
      Serial.println(currentClientIPAddress);
    }
  });
  server.begin();

  //Attach Timer Interrput to check IP Address
  checkIPAddress.attach(2, checkForIPAddress); //Use <strong>attach_ms</strong> if you need time in ms


  Serial.println("Setup Finished");
}


void loop()
{
  if (manualMode) {
    if (deviceIsConnected) {
      int leftMotor = angleToMotorSpeed(angleX);
      int rightMotor = leftMotor;
      //Serial.print(leftMotor);
      //Serial.println(leftMotor);
      double steer = angleToMotorSpeed(angleY) / 1023.0;
      if (steer < 0) {
        leftMotor = leftMotor + steer * leftMotor;
      } else {
        rightMotor = rightMotor - steer * rightMotor;
      }

      //Serial.print(leftMotor);
      //Serial.print(" ");
      //Serial.println(rightMotor);
      setMotorSpeed(leftMotor, rightMotor);
      //Serial.println(getUSDistance());;
    }
  } else {
    long distance = getUSDistance(); 
    Serial.println(distance);
    if ((distance <= 50) && (distance >= 30)) {
      setMotorSpeed(0,1023);
      delay(3000);
    } else if (distance < 30) {
      setMotorSpeed(-1023,-1023);
      delay(1000);
    } else {
      setMotorSpeed(1023,1023);
      delay(100);
    }
    
  }

}
