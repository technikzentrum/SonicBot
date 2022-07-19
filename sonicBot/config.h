#include <EEPROM.h>
int eepromMagic = 43;

typedef struct
{
  int deadBand = 500;
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
// ### Defines for H bridge ###
#define in1ALEDChannel 0
#define in2ALEDChannel 1
#define in1 19    
#define in2 18   
#define in3 21
#define in4 22
#define in1BLEDChannel 2
#define in2BLEDChannel 3
// ### Define Servo ###
#define servoPinH 16
#define servoPinG 17


//#define DEBUG           // Uncomment for non Serial Use

//#######################   Defines for Arduino OTA
//#define OTAUpdate //uncomment line to enable OTA update

/*void saveInEEPROM()
{
  #ifdef DEBUG
  Serial.println("Schreibe Daten");
  #endif
  configSet.validation = eepromMagic;
  #ifdef ESP32
    return EEPROM.write(0, configSet);
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
    return EEPROM.read(address, temp);
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
}*/
