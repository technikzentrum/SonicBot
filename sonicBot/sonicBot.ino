//#######################   Includes
#include "config.h"
#include "webserver.h"
#include "ota.h"
#include "motorcontrol.h"

//#######################   GLOBAL Varriables

// ### Sonnic Varriables ###
long duration = 0;
long distance = 0;
long last_tick = 0;
long last_tick_distance = 0;

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
  duration = pulseIn(ECHO, HIGH, 300000); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.
  distance = (duration / 2) * 0.03432; //Nun berechnet man die distance in Zentimetern. Man teilt zunächst die Zeit durch zwei (Weil man ja nur eine Strecke berechnen möchte und nicht die Strecke hin- und zurück). Den Wert multipliziert man mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.
  return distance;
}

//#######################   SETUP
void setup()
{
  #ifdef DEBUG
  Serial.begin (115200); //Start serial communication
  Serial.println("Setup Pindefines");
  #endif
  //Initialize H bridge
  initMotorPins();

  //Init US
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  // WLAN & Webserver
  initSPIFFS();
  initWebserver();
  
  //Arduino OTA
#ifdef OTAUpdate
  initOTA();
#endif

  #ifdef DEBUG
  Serial.println("Setup Finished");
  #endif
}


void loop()
{
  if (!manualMode) {
    /**
       TODO: Implement code for us-sensor
       The following functions might be helpful:
           long distance = getUSDistance();         //Get us-distance in cm
           Serial.println(distance);                //print distance to console
           setMotorSpeed(1023, 1023);               //set motor speed for left and right wheel
           delay(100)                               //Add additional delay
    */
  }
  if (last_tick_distance + 500 < millis()) {
    //getUSDistance();
      long distcnace = getUSDistance();// TODO: Use interruptand none Blocking mode
      changeCardText("sonnic", "Distance: " + String(distcnace));
      #ifdef DEBUG
        Serial.println("Distance: " + String(distcnace));
      #endif
    last_tick_distance = millis();
  }
  if (last_tick + 200 < millis()) {
    last_tick = millis();
    updateWebPage();
  }
  
#ifdef OTAUpdate
  OTAhandle();
#endif
}
