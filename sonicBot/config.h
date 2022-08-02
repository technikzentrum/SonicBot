#include <Preferences.h>
Preferences preferences;
int eepromMagic = 42;

typedef struct
{
  int deadBand = 600;
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
#define in1ALEDChannel 3
#define in2ALEDChannel 4
#define in1 19    
#define in2 18   
#define in3 21
#define in4 22
#define in1BLEDChannel 5
#define in2BLEDChannel 6
// ### Define Servo ###
#define servoPinH 16
#define servoPinG 17


//#define DEBUG           // Uncomment for non Serial Use

//#######################   Defines for Arduino OTA
//#define OTAUpdate //uncomment line to enable OTA update

void saveInEEPROM()
{
  #ifdef DEBUG
  Serial.println("Schreibe Daten");
  #endif
  configSet.validation = eepromMagic;
  int16_t myArray[sizeof(configSet)];
  ConfigLoad *ptemp = (ConfigLoad *) myArray; // cast the bytes into a struct ptr
  ptemp = &configSet;
  preferences.putBytes("struct", ptemp, sizeof(configSet));
}

void readOutEEPROM() {
  ConfigLoad myArray;
  ConfigLoad *ptemp = (ConfigLoad *) &myArray; // cast the bytes into a struct ptr
  #ifdef DEBUG
  Serial.println("lese Daten");
  #endif
  preferences.getBytes("struct", ptemp, sizeof(configSet));
  if (ptemp->validation != eepromMagic) {
    #ifdef DEBUG
      Serial.println("Keine Daten gefunden Schreibe neue.");
    #endif
    saveInEEPROM();
  } else {
    #ifdef DEBUG
      Serial.println("Übernehme Daten aus EEPROM");
    #endif
    configSet = myArray;
    #ifdef DEBUG
      Serial.println(configSet.ssid);
    #endif
  }
}

void initEEPROM()
{
  preferences.begin("config", false);
  readOutEEPROM();
}
