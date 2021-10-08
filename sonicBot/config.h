#include <EEPROM.h>
int eepromMagic = 43;

typedef struct
{
  int deadBand = 200;
  char ssid[33] = "TZL-Bot";
  char pw[65] = "";
  boolean invertLeftMotor = false;
  boolean invertRightMotor = false;
  boolean swapMotors = false;
  int validation = eepromMagic;
}
ConfigLoad;

ConfigLoad configSet;
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
// ### Defines for Sonic Module ###
#define TRIGGER 17
#define ECHO 16
#else
// ### Defines for H bridge ###
#define enA 4 //D2
#define in1 0 //D3   
#define in2 2 //D4   
#define in3 16 //D0
#define in4 D8 //S3
#define enB 5 //D1
// ### Defines for Sonic Module ###
#define TRIGGER D5
#define ECHO D6
#endif

//#define DEBUG           // Uncomment for non Serial Use

//#######################   Defines for Arduino OTA
//#define OTAUpdate //uncomment line to enable OTA update

void saveInEEPROM()
{
  #ifdef DEBUG
  Serial.println("Schreibe Daten");
  #endif
  configSet.validation = eepromMagic;
  #ifdef ESP32
    return EEPROM.writeBytes(0, configSet, len);
  #else
    EEPROM.put(0, configSet);
  #endif
  EEPROM.commit();    //Store data to EEPROM
}

void readOutEEPROM() {
  ConfigLoad temp;
  #ifdef DEBUG
  Serial.println("lese Daten");
  #endif
  #ifdef ESP32
    return EEPROM.readBytes(address, temp, sizeof(temp));
  #else
    EEPROM.get(0, temp);
  #endif
  if (temp.validation != eepromMagic) {
    #ifdef DEBUG
      Serial.println("Keine Daten gefunden Schreibe neue.");
    #endif
    saveInEEPROM();
  } else {
    #ifdef DEBUG
      Serial.println("Übernehme Daten aus EEPROM");
    #endif
    configSet = temp;
    #ifdef DEBUG
      Serial.println(configSet.ssid);
    #endif
  }
}

void initEEPROM()
{
  EEPROM.begin(512);  //Initialize EEPROM
  readOutEEPROM();
}
