#include <EEPROM.h>
int eepromMagic = 42;

typedef struct
{
  int deadBand = 200;
  char ssid[25] = "TZL-Bot";
  char pw[25] = "";
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

int writeBytes(int address, const void* value, size_t len)
{
  if (!value || !len)
    return 0;
  #ifdef ESP32
    return EEPROM.writeBytes(address, value, len);
  #else
    EEPROM.put(0, value);
  #endif
  return 1;
}

size_t readBytes(int address, void* value, size_t maxLen)
{
  if (!value)
    return 0;
  #ifdef ESP32
    return EEPROM.readBytes(address, value, maxLen);
  #else
    EEPROM.get(0, value);
  #endif
  return 1;
}

void saveInEEPROM()
{
  writeBytes(0, &configSet, sizeof(configSet));
  EEPROM.commit();    //Store data to EEPROM
}

void readOutEEPROM() {
  ConfigLoad temp;
  readBytes(0, &temp, sizeof(temp));
  if (temp.validation != eepromMagic) {
    saveInEEPROM();
  } else {
    configSet = temp;
  }
}

void initEEPROM()
{
  EEPROM.begin(sizeof(configSet)+2);  //Initialize EEPROM
  readOutEEPROM();
}
