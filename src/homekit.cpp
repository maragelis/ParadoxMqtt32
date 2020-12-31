#include <homekit.h>

char jsonMsg[256];
int zones[32];
int ParadoxInit;

homebridge hb;

 const char* toAdd = "homebridge/to/add";
 const char* addService = "homebridge/to/add/service";
 const char* toRemove = "homebridge/to/remove";
 const char* removeService= "homebridge/to/remove/service";
 const char* toGet= "homebridge/to/get";
 const char* toSet = "homebridge/to/set";
 const char* setReachability= "homebridge/to/set/reachability";
 const char* setAccessoryinformation= "homebridge/to/set/accessoryinformation";
 const char* fromGet= "homebridge/from/get";
 const char* fromSet= "homebridge/from/set";
 const char* fromResponse = "homebridge/from/response";
 const char* fromIdentify = "homebridge/from/identify";



void initHomkitService()
{
    readHomekitConfig();
    
    if(ParadoxInit!=1)
    {
        struct homebridgeMSG msg;
        strcpy(msg.service , "SecuritySystem");
        strcpy(msg.service_name , HomekitDeviceName);
        strcpy(msg.name , HomekitDeviceName);
        
        strcpy(hb.topic , toAdd);
        strcpy(hb.jsonstr , jsonMsg);
        ParadoxInit = 1;
        saveHomekitConfig();
    }    
    
}


void getJsonObject(homebridgeMSG msg)
{
        
        DynamicJsonDocument json(256);
        json["service"] = msg.service;
        json["service_name"] = msg.service_name;
        json["name"] = msg.name;
        serializeJson(json,jsonMsg);       
      
}

void saveHomekitConfig()
{
    StaticJsonDocument<256> json;
    JsonArray array = json["zones"].to<JsonArray>();

    File configFile = SPIFFS.open("/Homeconfig.json", "w");
      if (!configFile) {
       
      }
      else
      {
            json["ParadoxInit"] = ParadoxInit;
            for(int v : zones) {
            array.add(v);
            }
            serializeJson(json,configFile);
            configFile.close();
      }
      

      
}

void readHomekitConfig()
{
    if (SPIFFS.begin(true)) 
    {
    
        if (SPIFFS.exists("/Homeconfig.json")) 
        {
            File configFile = SPIFFS.open("/Homeconfig.json", "r");
            if (configFile) 
            {
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument json(1024);
                auto deserializeError = deserializeJson(json, buf.get(), size);
                if (!deserializeError) 
                {
                    ParadoxInit = json["ParadoxInit"].as<int>();
                   memset(zones, 0, sizeof(zones));
                   JsonArray zonearray = json["zones"].as<JsonArray>(); 
                   int cnt=0;
                   for(JsonVariant v : zonearray) {
                        zones[cnt++]=v.as<int>();
                    }
                }
            }
        }
    }
}