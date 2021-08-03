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

// ### User Variables
#define BOT_NAME "myBot2" // <---- change me
#define BOT_PASSWORD "" // <---- change me
#define DEBUG           // Uncomment for non Serial Use

//#######################   Defines for Arduino OTA
#define OTAUpdate //uncomment line to enable OTA update
