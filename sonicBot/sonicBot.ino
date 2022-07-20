//#######################   Includes
#include "config.h"
#include "webserver.h"
#include "ota.h"
#include "motorcontrol.h"

//#######################   GLOBAL Varriables

// ### Timer Eventsorces
#ifdef DEBUG
#define EVENT_INTERVALL 500
#else
#define EVENT_INTERVALL 200
#endif
long last_tick = 0;


//#######################   SETUP
void setup()
{
  #ifdef DEBUG
  Serial.begin (115200); //Start serial communication
  Serial.println("Setup Pindefines");
  #endif
  //Initialize H bridge
  initMotorPins();

  initEEPROM();
  
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
  if (last_tick + EVENT_INTERVALL < millis()) {
    last_tick = millis();
    updateWebPage();
  }
  if (lastServo + Servo_INTERVALL < millis()) {
    //TODO: Disable Servo
  }
#ifdef OTAUpdate
  OTAhandle();
#endif
}
