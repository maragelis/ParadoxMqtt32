#ifndef main_h
#define main_h

#include <Arduino.h>
#include <stdio.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <FS.h>   
#include <SPIFFS.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "PubSubClient.h"
#include "WiFiManager.h" 
#include <HardwareSerial.h>
#include <AsyncTCP.h>
#include <ESP32SSDP.h>
#include <ParadoxEvents.h>
#include <WiFiMulti.h>
#include <time.h>
#include <SimpleTimer.h>






#define firmware "PARADOX32_2021.08.12.16"


#define mqtt_server       "192.168.2.230"
#define mqtt_port         "1883"
#define mqtt_user         ""
#define mqtt_password     "" 

#define Hostname          "paradox32CTL" //not more than 15


#define timezone 2 //for setdate command
const char* ntpServer = "pool.ntp.org";


#define Stay_Arm  0x01
#define Stay_Arm2 0x02
#define Sleep_Arm 0x03
#define Full_Arm 0x04
#define Disarm  0x05
#define Bypass 0x10
#define PGMon 0x32
#define PGMoff 0x33

#define MessageLength 37

#define LED LED_BUILTIN


bool USE6DigitCode = false;
//If esp is dev kit then set to 1 else 0
bool ESPDEVKit = 1;

bool Hassio= 1; // 1 enables 0 disables Hassio-Openhab support
bool HomeKit= 1 ;// enables homekit topic
bool SendAllE0events =1 ;//If you need all events set to 1 else 0 
bool usePartitions= 0; //If you use partitions enable this to get partition number in Hassio topic 

int ArmStateRefresh = 0 ; //will send arm state every 30 seconds dont use smaller then 30, 0 to disable;

bool TRACE = 0;
bool OTAUpdate = 1;


#define def_topicOut "out" 
#define def_topicStatus "status"
#define def_topicIn "in"
#define def_topicHassioArm "hassio/Arm"
#define def_topicHassio "hassio"
#define def_topicArmHomekit "HomeKit"

#define USE_LITTLEFS    true
#define _WIFIMGR_LOGLEVEL_    0

//If you need event decriptions set to 1 else 0 Can cause slow downs on heavy systems.
//Can also be enabled by sending sendeventdescriptions=1 to in topic.
//Enable it here if you want it enabled after a reboot
bool SendEventDescriptions =1;

enum SecuritySystemCurrentStates
{
    STAY_ARM=0,AWAY_ARM = 1,NIGHT_ARM = 2,DISARMED = 3,ALARM_TRIGGERED = 4
};

enum SecuritySystemStates
{
    SecuritySystemTargetState=0,SecuritySystemCurrentState=1
};

void serial_flush_buffer();
void handleMqttKeepAlive();
void sendMQTT(String topicNameSend, String dataStr,bool  retain);
void sendCharMQTT(char* topic, char* data , bool retain);
void subscribing(char* topicName);
void readSerial();
void answer_E0();
boolean reconnect();
struct inPayload Decodejson(char *Payload);
void doLogin(inPayload _inpayload);//byte pass1, byte pass2);
void ControlPanel(inPayload data);
void ArmState();
void PanelStatus0();
void PanelStatus1();
String getpage();
void panelSetDate();
void sendHomeBridgeState(int _partition, SecuritySystemStates _SystemState, SecuritySystemCurrentStates _SystemCurrentState);



#endif /*main_h*/
