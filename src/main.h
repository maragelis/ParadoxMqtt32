#ifndef main_h
#define main_h

#include <Arduino.h>
#include <stdio.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <FS.h>   
#include <SPIFFS.h>
#include <SPI.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <WebServer.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "PubSubClient.h"
#include "ESP_WiFiManager.h" 
#include <HardwareSerial.h>
#include <AsyncTCP.h>

#include <WebServer.h>
#include <ESP32SSDP.h>
#include <DNSServer.h>
#include <ParadoxEvents.h>
#include <WiFiMulti.h>
#include <time.h>
#include <homekit.h>






#define firmware "PARADOX32_2020.12.31.16"

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

#define Uart0_RX 
#define TTLCD 



//If esp is dev kit then set to 1 else 0
bool ESPDEVKit = 1;

bool Hassio= 1; // 1 enables 0 disables Hassio-Openhab support
bool HomeKit= 1 ;// enables homekit topic
bool SendAllE0events =1 ;//If you need all events set to 1 else 0 
bool usePartitions= 1; //If you use partitions enable this to get partition number in Hassio topic 

int ArmStateRefresh = 30 ; //will send arm state every 30 seconds dont use smaller then 30, 0 to disable;

bool TRACE = 0;
bool OTAUpdate = 1;
bool HasTFTScreen  = 0;

#define def_topicOut "out" 
#define def_topicStatus "status"
#define def_topicIn "in"
#define def_topicHassioArm "hassio/Arm"
#define def_topicHassio "hassio"
#define def_topicArmHomekit "HomeKit"

#define USE_LITTLEFS    true
#define _WIFIMGR_LOGLEVEL_    0

//#define ParadoxGSMInstalled  // Comment out to use PARADOX GSMModule
//#define Sim800Instaled //
//#define HasTFTScreen //



#ifdef HasTFTScreen
#include <tftScreen.h>
HasTFTScreen =1;
#endif




#ifdef ParadoxGSMInstalled
#define GSMModuleRX  GPIO_NUM_26 //RX pin of GSM or Dialer Module 
#define GSMModuleTX  GPIO_NUM_27 //TX pin of GSM or Dialer Module 
#else
#ifdef Sim800Instaled

#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024    // Set RX buffer to 1Kb

const char simPIN[] = ""; 

#include <TinyGsmClient.h>

#endif
#endif



//If you need event decriptions set to 1 else 0 Can cause slow downs on heavy systems.
//Can also be enabled by sending sendeventdescriptions=1 to in topic.
//Enable it here if you want it enabled after a reboot
bool SendEventDescriptions =1;


void serial_flush_buffer();
void handleMqttKeepAlive();
void sendMQTT(String topicNameSend, String dataStr,bool  retain);
void sendCharMQTT(char* topic, char* data , bool retain);
void subscribing(char* topicName);
void readSerial();
void answer_E0();
boolean reconnect();
struct inPayload Decodejson(char *Payload);
void doLogin(byte pass1, byte pass2);
void ControlPanel(inPayload data);
void ArmState();
void PanelStatus0();
void PanelStatus1();
String getpage();
void panelSetDate();


#endif /*main_h*/
