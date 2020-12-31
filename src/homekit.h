#ifndef _homekit_
#define _homekit_

#include <Arduino.h>
#include <stdio.h>
#include <ArduinoJson.h>
#include <FS.h>   
#include <SPIFFS.h>

#define hombridgeRootTopic "homebridge"
#define HomekitDeviceName "Paradox_Alarm_system"



struct homebridge{
     char topic[128];
     char jsonstr[256];
 } ;

struct homebridgeMSG
{
    //{"name": "flex_lamp", "service_name": "light", "service": "Switch"}
    char name[50];
    char service_name[50];
    char service[50];
};

extern const char* toAdd ; //= "homebridge/to/add";
extern const char* addService ; //= "homebridge/to/add/service";
extern const char* toRemove ; //= "homebridge/to/remove";
extern const char* removeService; //= "homebridge/to/remove/service";
extern const char* toGet; //= "homebridge/to/get";
extern const char* toSet ; //= "homebridge/to/set";
extern const char* setReachability;//= "homebridge/to/set/reachability";
extern const char* setAccessoryinformation; //= "homebridge/to/set/accessoryinformation";
extern const char* fromGet; //= "homebridge/from/get";
extern const char* fromSet; //= "homebridge/from/set";
extern const char* fromResponse;// = "homebridge/from/response";
extern const char* fromIdentify;// = "homebridge/from/identify";

void initHomkitService();
void getJsonObject(homebridgeMSG msg);
void readHomekitConfig();
void saveHomekitConfig();



#endif //_homekit_