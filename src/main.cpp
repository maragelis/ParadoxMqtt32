
#include <main.h>

const long  gmtOffset_sec =  3600 * timezone;

HardwareSerial ParadoxSerial(2);
HardwareSerial Debug(0);
HardwareSerial GSMModule(1);


WiFiClient espClient;

PubSubClient client(espClient);

bool shouldSaveConfig = false;
bool ResetConfig = false;
bool PanelConnected =false;
bool PanelError = false;
bool RunningCommand=false;
bool JsonParseError=false;
bool initState=false;
 
char inData[38]; // Allocate some space for the string
byte pindex = 0; // Index into array; where to store the character

long lastReconnectAttempt = 0;



WebServer HTTP(80);


WiFiMulti wifiMulti;

struct inPayload
{
  byte PcPasswordFirst2Digits;
  byte PcPasswordSecond2Digits;
  byte PcPasswordThird2Digits;
  byte Command;
  byte Subcommand;
 } ;
 
 typedef struct {
  bool Fire;
  bool Audible;
  bool Silent;
  bool AlarmFlg;
  bool StayFlg;
  bool SleepFlg;
  bool ArmFlg;
  bool zoneisbypassed;
  int Partition;
 } PanelStatus1msg;

 typedef struct {
     int intArmStatus;
     String stringArmStatus;
     int Partition;
     int sent;
     int TargetState;
 } paradoxArm;

 //paradoxArm hassioStatus0;
 //paradoxArm hassioStatus1;
 
 //paradoxArm homekitStatus0;
 //paradoxArm homekitStatus1;
 
 paradoxArm hassioStatus[2];
 paradoxArm homekitStatus[2];
 PanelStatus1msg _PanelStatus1msg;

 SimpleTimer timer;



 
 
byte checksumCalculate(byte checksum) {
    while (checksum > 255) {
      checksum = checksum - (checksum / 256) * 256;
    }
  return checksum & 0xFF;
}

char root_topicStatus[50] ;
char root_topicOut[50] ;
char root_topicIn[50];
char root_topicHassioArm[50];
char root_topicHassio [50];
char root_topicArmHomekit [50];
char LWT[50] ;

char Mqtt_ServerAddress[50];
char Mqtt_ServerPort[50];
char Mqtt_Username[50];
char Mqtt_Password[50];




void trc(String msg){
  if (TRACE) {
  Debug.println(msg);
  //sendMQTT(root_topicStatus,msg,false);
  }
}

void traceInData(){
  if (TRACE && (inData[0] & 0xF0) != 0xE0)
  {
    for (int i=0;i<MessageLength;i++)
    {
      Debug.print("Address-");
      Debug.print(i);
      Debug.print("=");
      Debug.println(inData[i], HEX);

    }
      
    
  }
}





void handleMqttKeepAlive(){

  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client.loop();
  }
}

void SetupMqttServer()
{
  strcpy(Mqtt_ServerAddress, mqtt_server);
  strcpy(Mqtt_ServerPort,mqtt_port);
  strcpy(Mqtt_Username, mqtt_user);
  strcpy(Mqtt_Password, mqtt_password);
}

void SetupMqttTopics()
{
  sprintf(root_topicStatus, "%s/%s", Hostname, def_topicStatus );
  sprintf(root_topicIn, "%s/%s", Hostname, def_topicIn );

  sprintf(root_topicArmHomekit, "%s/%s", Hostname, def_topicArmHomekit );
  sprintf(root_topicHassio, "%s/%s", Hostname, def_topicHassio );
  sprintf(root_topicOut, "%s/%s", Hostname, def_topicOut );
  sprintf(root_topicHassioArm, "%s/%s", Hostname, def_topicHassioArm );
  sprintf(LWT,"%s/LWT",Hostname);

  //root_topicHassioArm
  //
} 

void subscribing(char* topicName){ // MQTT subscribing to topic
  //char topicStrRec[26];
  //topicNameRec.toCharArray(topicStrRec,26);
  // subscription to topic for receiving data
  boolean pubresult = client.subscribe(topicName);
  if (pubresult) {
    trc("subscription OK to");
    trc(String(topicName));
  }
}

void StartSSDP(){
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    Debug.printf("Starting HTTP...\n");

   
    HTTP.on("/description.xml", HTTP_GET,  []() {
      SSDP.schema(HTTP.client());
    });

   

     HTTP.on("/", HTTP_GET, []() {
      HTTP.send(200, "text/plain", Hostname);
    });
    HTTP.begin();
/*
 HTTP.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(getpage()));
    });

    HTTP.on("/index.html", HTTP_GET, []() {
      HTTP.send(200, "text/html", getpage());
    });
    HTTP.on("/", HTTP_GET, []() {
      HTTP.send(200, "text/plain", Hostname);
    });

    
    HTTP.begin();
*/
    Debug.printf("Starting SSDP...\n");
    SSDP.setSchemaURL(F("description.xml"));
    SSDP.setDeviceType(F("upnp:rootdevice"));
    SSDP.setHTTPPort(80);
    SSDP.setName(Hostname);
    SSDP.setSerialNumber(WiFi.macAddress());
    SSDP.setURL(String("http://") + WiFi.localIP().toString().c_str() +"/index.html");
    SSDP.setModelName(F("ESP32"));
    SSDP.setModelNumber(firmware);
    SSDP.setModelURL(F("https://github.com/maragelis/ParadoxMqtt32"));
    SSDP.setManufacturer(F("PM ELECTRONICS"));
    SSDP.setManufacturerURL(F("https://github.com/maragelis/"));
    SSDP.begin();

    if (!MDNS.begin(Hostname)) {
    trc(F("Error setting up MDNS responder!"));
    while (1) {
      delay(1000);
    }
  }
    trc(F("mDNS responder started"));

  

  // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);

    trc(F("Ready!\n"));
  }
  
}

void readSerialQuick(){
 while (ParadoxSerial.available()<37  )  
  { 
    yield(); 
  }                            
  {
    memset(inData,0,sizeof(inData));
    trc("Reading readSerialQuick");
      pindex=0;
  
      while(pindex < 37) // Paradox packet is 37 bytes 
      {

          inData[pindex++]=ParadoxSerial.read();  
      } 
      inData[++pindex]=0x00; // Make it print-friendly
      trc("readSerialQuick data is");
      traceInData();
  }
}

void updateArmStatus(byte event, byte sub_event, byte partition){
  
  homekitStatus[partition].Partition = partition;
  hassioStatus[partition].Partition = partition;
  

 

  if (event == 2)
  {
    
    switch (sub_event)
    {
      case 4:
        hassioStatus[partition].stringArmStatus = "triggered";
        homekitStatus[partition].stringArmStatus = "ALARM_TRIGGERED";
        homekitStatus[partition].intArmStatus=(int)SecuritySystemCurrentStates::ALARM_TRIGGERED;
        
       
        
      break;

      case 11:
      if (TRACE)
      Debug.println("Disarmed set by updateArmStatus");
        hassioStatus[partition].stringArmStatus = "disarmed";
        homekitStatus[partition].stringArmStatus = "DISARMED";
        homekitStatus[partition].intArmStatus = SecuritySystemCurrentStates::DISARMED;
        
        
        break;

      case 12:
         hassioStatus[partition].stringArmStatus = "armed_away";
         homekitStatus[partition].stringArmStatus = "AWAY_ARM";
         homekitStatus[partition].intArmStatus = SecuritySystemCurrentStates::AWAY_ARM;
         
         
        break;

        case 14:
         hassioStatus[partition].stringArmStatus = "pending";
         
        break;

      default : break;
    }
  }
  else if (event == 6)
  {
    if (sub_event == 3)
    {
      
      hassioStatus[partition].stringArmStatus = "armed_home";
      homekitStatus[partition].stringArmStatus = "STAY_ARM";
      homekitStatus[partition].intArmStatus = SecuritySystemCurrentStates::STAY_ARM;
      
    }
    else if ( sub_event == 4)
    {
      
      hassioStatus[partition].stringArmStatus = "armed_home";
      homekitStatus[partition].stringArmStatus = "NIGHT_ARM";
      homekitStatus[partition].intArmStatus = SecuritySystemCurrentStates::NIGHT_ARM; 
    }
  }
  
        
}
void sendMQTT(String topicNameSend, String dataStr,bool  retain){
    handleMqttKeepAlive();
    char topicStrSend[256];
    topicNameSend.toCharArray(topicStrSend,256);
    char dataStrSend[200];
    dataStr.toCharArray(dataStrSend,256);
    boolean pubresult = client.publish(topicStrSend,dataStrSend ,retain);
    if (TRACE)
     {
      Debug.print("Sent:");
      Debug.print( "\"" + dataStr + "\"");
      Debug.print(" to Topic:");
      Debug.println(topicNameSend);
      Debug.print("with pubresult :");
      Debug.println(pubresult);
    }
}


void sendHomeBridgeState(int _partition,SecuritySystemStates _SystemState,SecuritySystemCurrentStates _SystemCurrentState)
{
  char output[256];
  char topic[100];
  DynamicJsonDocument root(128);
   //root["characteristic"]=
   String characteristic = _SystemState==SecuritySystemStates::SecuritySystemCurrentState?"SecuritySystemCurrentState":"SecuritySystemTargetState";
   root[characteristic]=_SystemCurrentState;
   
   serializeJson(root,output);
   sprintf(topic, "%s/%d/armState", root_topicArmHomekit,_partition);
   sendCharMQTT(topic,output, false); 


}



void sendArmStatus(){
  //char output[256];
  DynamicJsonDocument root(128);
  
  char ZoneTopic[128];
  //StaticJsonBuffer<128> jsonBuffer;
  //JsonObject& root = jsonBuffer.createObject();
        if (Hassio)
        {
          
          sprintf(ZoneTopic, "%s/%d",root_topicHassioArm,hassioStatus[0].Partition);
          sendMQTT(ZoneTopic,hassioStatus[0].stringArmStatus, true);  
          if (usePartitions)
          {
           
            sprintf(ZoneTopic, "%s/%d",root_topicHassioArm,hassioStatus[1].Partition);
            sendMQTT(ZoneTopic,hassioStatus[1].stringArmStatus, true);  

          }
          
              
        }
        if (HomeKit )
        {
          //{"name":"Paradox",service_name="Paradox" ,"characteristic":"SecuritySystemCurrentState","value": 1 }
          //root["name"] = HomekitDeviceNameP0;
          //root["service_name"]= HomekitDeviceNameP0;
          //root["characteristic"]="SecuritySystemCurrentState";
          //root["value"]=homekitStatus[0].intArmStatus;
          //root["ArmStatusD"]=homekitStatus[0].stringArmStatus ;
          //sprintf(ZoneTopic, "%s/%d",homebridgeRootTopic,HB_toSet,homekitStatus[0].Partition);
          //serializeJson(root,output);
          //sendCharMQTT(ZoneTopic,output, false); 
          
          sendHomeBridgeState(homekitStatus[0].Partition,SecuritySystemStates::SecuritySystemCurrentState  ,static_cast<SecuritySystemCurrentStates>(homekitStatus[0].intArmStatus));
          
          
          //root.printTo(output);
          if (usePartitions)
          {
            //root["Armstatus"]=homekitStatus[1].intArmStatus;
            //root["ArmStatusD"]=homekitStatus[1].stringArmStatus ;
           //// p2["Partition"]=homekitStatus[1].Partition;          
            //sprintf(ZoneTopic, "%s/%d",root_topicArmHomekit,homekitStatus[1].Partition);
            //serializeJson(root,output);
            //sendCharMQTT(ZoneTopic,output, false); 
            
            sendHomeBridgeState(homekitStatus[1].Partition,SecuritySystemStates::SecuritySystemCurrentState,static_cast<SecuritySystemCurrentStates>(homekitStatus[1].intArmStatus));
          
          }
          
        }
}

void sendACFail(bool acfailure)
{
  DynamicJsonDocument json(64);
  char doc[64];
  json["ACfailure"]=acfailure;
  char topic[128];
  sprintf(topic, "%s/%s", root_topicStatus,"ACFailure");
  serializeJson(json,doc);
  sendCharMQTT(topic,doc,false);


}

void processMessage( byte event, byte sub_event, byte partition , String dummy ){
  char ZoneTopic[128];
  if ( (event == 44 || event == 45) && sub_event==1)
  {
      bool acfail = event==44?true:false;
      sendACFail(acfail);
  }
  
  if ((Hassio || HomeKit) && (event == 2 || event == 6))
  {
    updateArmStatus(event,sub_event, partition); 
  }

  //Dont send the arm event now send it on next message, because it might be updated to sleep or stay.
  if ((Hassio || HomeKit) &&  (event != 2 and sub_event != 12) )  
  {  
    if (homekitStatus[partition].sent != homekitStatus[partition].intArmStatus)
    {
      sendArmStatus();
      homekitStatus[partition].sent = homekitStatus[partition].intArmStatus;
      }
  }

   if ((Hassio || HomeKit) && (event == 29))
  {
      sendArmStatus();
      homekitStatus[partition].sent = homekitStatus[partition].intArmStatus;
  }
 
  if ((Hassio ) && (event == 1 || event == 0))
  {
    
    if (usePartitions)
    {
      sprintf(ZoneTopic, "%s/%d/zone%d",root_topicHassio,partition,sub_event);
    }
    else
    {
      sprintf(ZoneTopic, "%s/zone%d",root_topicHassio,sub_event);
    }
    
    //zone.toCharArray(ZoneTopic, 80);
    //zone = String(ZoneTopic) + String(sub_event);
    //zone.toCharArray(ZoneTopic, 80);

    String zonestatus = event==1?"ON":"OFF";

    sendMQTT(ZoneTopic, zonestatus, true);    
  }
  
  if ((HomeKit ) && (event == 1 || event == 0))
  {
    char output[128];
    
    DynamicJsonDocument homekitmsg(128);

    //StaticJsonBuffer<128> jsonBuffer;
    //JsonObject& homekitmsg = jsonBuffer.createObject();
    homekitmsg["zone"]=sub_event;
    dummy.trim();
    homekitmsg["zoneName"]=String(dummy);
    homekitmsg["partition"]=partition;
    homekitmsg["state"]=event==1?true:false;
    serializeJson(homekitmsg,output);
    sprintf(ZoneTopic,"%s/zone%d",root_topicArmHomekit,sub_event);
    sendCharMQTT(ZoneTopic,output,false); 
  }

  if (SendAllE0events)
  {
    char outputMQ[256];
     DynamicJsonDocument root(256);
    //StaticJsonBuffer<256> jsonBuffer;
    //JsonObject& root = jsonBuffer.createObject();
    root["event"]=event;
    root["sub_event"]=sub_event;
    root["partition"]=partition;
    if (SendEventDescriptions)
    {
      root["sub_eventD"]=getSubEvent(event,sub_event);
      root["eventD"]=getEvent(event);
    }
    
    root["data"]=dummy;
    //root.printTo(outputMQ);
    serializeJson(root,outputMQ);
    
    sendCharMQTT(root_topicOut,outputMQ,false); 
  }
}




void sendCharMQTT(char* topic, char* data , bool retain){
  handleMqttKeepAlive();
  if (TRACE)
  {
    Debug.print("Sending MQmessage to topic: ");
    Debug.println(topic);
    Debug.print("With data: ");
    Debug.println(data);
  }
  client.publish(topic, data, retain);
  
}

void readSerial(){
  
    memset(inData,0,sizeof(inData));
    trc("Reading main loop");
    pindex=0;
  
    while(pindex < 37) // Paradox packet is 37 bytes 
    {
      int serialdata = ParadoxSerial.read();  

      inData[pindex++]=serialdata;

    } 
    inData[++pindex]=0x00; // Make it print-friendly

    if ((inData[0] & 0xF0) == 0xE0)
    { 
      trc(F("start  answer_E0"));
      answer_E0();  
    }
    
      traceInData();   
 
}

void answer_E0(){
                
  String zlabel=" ";

  if (inData[14] != 1)
  {
    for (int k = 15; k <= 30; k++)
    {
      zlabel = zlabel + String(inData[k]);
    }
    zlabel.trim();
  }
  
  processMessage( inData[7], inData[8], inData[9], zlabel);
  if (inData[7] == 48 && inData[8] == 3)
  {
    PanelConnected = false;
    trc(F("Recieved PanelConnected = false"));
  }
  else if (inData[7] == 48 && inData[8] == 2 )
  {
    PanelConnected = true;
      trc(F("Recieved PanelConnected = true"));
  }
}

void blink(int duration) {
   
  digitalWrite(LED_BUILTIN,LOW);
  delay(duration);
  digitalWrite(LED_BUILTIN,HIGH);
 
}

void saveConfigCallback () {
  trc(F("Should save config"));
  shouldSaveConfig = true;
}

void callback(char* topic, byte* payload, unsigned int length) {
    if (RunningCommand){
     trc("Command already Running exiting");
      return;
    }
  trc(F("Hey I got a callback "));
  // Conversion to a printable string
  payload[length] = '\0';
  inPayload data;
  
  trc("JSON Returned! ====");
  String progEvent = String((char *)payload);
  trc(progEvent);

  if (progEvent.indexOf("=")>0)
  {
    progEvent.toLowerCase();
  
    if (progEvent == "trace=1")
    {
      TRACE=1;
      Debug.println("Trace is ON");
    }
    else if (progEvent == "trace=0")
    {
      TRACE=0;
      Debug.println("Trace is OFF");
      
    }
    else if (progEvent == "ota=0")
    {
      OTAUpdate=0;
      Debug.println("OTA update is OFF");
      
    }
    else if (progEvent == "ota=1")
    {
      OTAUpdate=1;
      Debug.println("OTA update is ON");
      
    }
    else if (progEvent == "resetwifi=1")
    {
      Debug.println("Resetting WIFI Settings");
      wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //load the flash-saved configs
        esp_wifi_init(&cfg); //initiate and allocate wifi resources (does not matter if connection fails)
        delay(2000); //wait a bit
        if(esp_wifi_restore()!=ESP_OK)
        {
            Debug.println("WiFi is not initialized by esp_wifi_init ");
         }else{
             Debug.println("WiFi Configurations Cleared!");
         }
         //continue
        delay(1000);
        ESP.restart();
    }
    else if (progEvent == "sendeventdescriptions=1")
    {
      SendEventDescriptions = 1;
      Debug.println("SendEventDescriptions is ON");
      
    }
    else if (progEvent == "sendeventdescriptions=0")
    {
      SendEventDescriptions = 0;
      Debug.println("SendEventDescriptions is OFF");
      
    }
    
    else 
    {
      trc(F("error in ProgEvent payload "));
      
    }
    return ;
  }
    
  trc(F("parsing Recievied Json Data"));
  data = Decodejson((char *)payload);
  if (JsonParseError)
  {
    trc(F("Error parsing Json Command") );
    JsonParseError=false;
    return;
  }
  trc(F("Json Data is ok "));
  PanelError = false;
    RunningCommand=true;
  if (!PanelConnected)
  {
    trc(F("Panel not logged in"));
    doLogin(data);//.PcPasswordFirst2Digits, data.PcPasswordSecond2Digits);
    trc(PanelConnected?"Panel logged in":"Panel login failed");
  }
      
 
  if (!PanelConnected)
  {
    trc(F("Problem connecting to panel"));
    sendMQTT(root_topicStatus, "{\"status\":\"Problem connecting to panel\"}" , false);
  }else if (data.Command == 0x50  ) 
  {
    trc(F("Running panel status command"));
    if (data.Subcommand==0)
    {
     PanelStatus0();
    }
    if (data.Subcommand==1)
    {
     PanelStatus1();
    }
  }
  else if (data.Command == 0x91  )  {
    trc(F("Running ArmState"));
    sendArmStatus();
  }
  else if (data.Command == 0x30)
  {
    trc(F("Command Setdate"));
    panelSetDate();
  }
  
  else if (data.Command != 0x00  )  {
    trc(F("Running Command"));
    
    
    if (HomeKit)
    {
      SecuritySystemCurrentStates ts=SecuritySystemCurrentStates::DISARMED;
      if (data.Command == Stay_Arm)
      {          
        ts=SecuritySystemCurrentStates::STAY_ARM;
      }
      else if (data.Command == Sleep_Arm )
      {
        ts=SecuritySystemCurrentStates::NIGHT_ARM;
      }
      else if (data.Command == Full_Arm )
      {
        ts=SecuritySystemCurrentStates::AWAY_ARM;
      }
      else if (data.Command == Disarm )
      {
        ts=SecuritySystemCurrentStates::DISARMED;
      }
      homekitStatus[data.Subcommand].TargetState = ts;
      sendHomeBridgeState(data.Subcommand,SecuritySystemStates::SecuritySystemTargetState,ts);      
    }    
    ControlPanel(data);
  } 
  else  {
    trc(F("Bad Command "));
    sendMQTT(root_topicStatus, "{\"status\":\"Bad Command\" }", false);
  }
  
  RunningCommand=false;
  
}

byte getPanelCommand(String data){
  byte retval=0x00;

  data.toLowerCase();
  if (data == "stay" || data=="0")
  {
    retval = Stay_Arm; 
  }
  else if (data == "arm" || data=="1")
  {    
    retval= Full_Arm;
  }
  else if (data == "sleep" || data=="2")
  {
    retval= Sleep_Arm; 
  }
  else if (data == "disarm" || data == "3")
  {
   retval=Disarm;  
  }
  else if (data == "bypass" || data == "10")
  {
    retval=Bypass; 
  }
  else if (data == "pgm_on" || data == "pgmon")
  {
    retval = PGMon;
  }
  else if (data == "pgm_off" || data == "pgmoff")
  {
    retval = PGMoff;
  }
  else if (data == "panelstatus" )
  {
    retval=0x50;
  }
  else if (data == "setdate")
  {
    retval=0x30; 
  }
  else if (data == "armstate")
  {
    retval=0x91;
  }
  
    if(TRACE)
    {
      Debug.print("returned command = ");
      Debug.println(retval , HEX);
    }
  return retval;
}

void panelSetDate(){
  
    struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Debug.println("Failed to obtain time");
    trc(F("ERROR getting NTP Date "));
     sendMQTT(root_topicStatus,"{\"status\":\"ERROR getting NTP Date  \" }", false);
    return;
  }
  else
  {
    
  Debug.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
   //dateTime = NTPch.getNTPtime(timezone, 1);
  
   
    
     byte actualHour = timeinfo.tm_hour  & 0xFF;
     byte actualMinute = timeinfo.tm_min  & 0xFF;
     byte actualyear = (timeinfo.tm_year - 100) & 0xFF ;
     byte actualMonth = (timeinfo.tm_mon+1)  & 0xFF;
     byte actualday = timeinfo.tm_mday  & 0xFF;

    

     byte data[MessageLength] = {};
     byte checksum;
     memset(data, 0, sizeof(data));

     data[0] = 0x30;
     data[4] = 21 & 0xFF;         //Century
     data[5] = actualyear;   //Year
     data[6] = actualMonth;  //Month
     data[7] = actualday;    //Day
     data[8] = actualHour;   //Time
     data[9] = actualMinute; // Minutes
     data[33] = 0x05;

     checksum = 0;
     for (int x = 0; x < MessageLength - 1; x++)
     {
       checksum += data[x];
     }

     data[36] = checksumCalculate(checksum);
      trc("sending setDate command to panel");
      ParadoxSerial.write(data, MessageLength);
  
if (TRACE)
    {
      Debug.println(timeinfo.tm_hour);
      Debug.println(timeinfo.tm_min);
      Debug.println(timeinfo.tm_year);
       Debug.println(timeinfo.tm_yday);
       Debug.println(timeinfo.tm_wday);
      Debug.println(timeinfo.tm_mon);
      Debug.println(timeinfo.tm_mday);
      Debug.println("***************");
    

      for (int i=0;i<MessageLength;i++)
        {
          Debug.print("Address-");
          Debug.print(i);
          Debug.print("=");
          Debug.println(data[i], HEX);
          

        }
    } 
     //readSerialQuick();
  
  } 

}

void ControlPanel(inPayload data){
  byte armdata[MessageLength] = {};
  byte checksum;

  memset(armdata, 0, sizeof(armdata));

  armdata[0] = 0x40;
  armdata[2] = data.Command;
  armdata[3] = data.Subcommand;
  armdata[33] = 0x05;
  armdata[34] = 0x00;
  armdata[35] = 0x00;
  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += armdata[x];
  }
  armdata[36] = checksumCalculate(checksum);
  
  trc(F("sending Arm command to panel"));
  ParadoxSerial.write(armdata, MessageLength);
  
}

void PanelDisconnect(){
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));

  data[0] = 0x70;
  data[2] = 0x05;
  data[33] = 0x05;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);  
  ParadoxSerial.write(data, MessageLength);
  
}

void PanelStatus0(){
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));

  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x00;
  data[33] = 0x05;
 checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  data[36] = checksumCalculate(checksum);
  trc(F("sending Panel Status 0 command to panel"));
  ParadoxSerial.write(data, MessageLength);  


   readSerialQuick();
   bool Timer_Loss = bitRead(inData[4],7);
    bool PowerTrouble  = bitRead(inData[4],1);
    bool ACFailureTroubleIndicator = bitRead(inData[6],1);
    bool NoLowBatteryTroubleIndicator = bitRead(inData[6],0);
    bool TelephoneLineTroubleIndicator = bitRead(inData[8],0);
    int ACInputDCVoltageLevel = inData[15];
    int PowerSupplyDCVoltageLevel =inData[16];
    int BatteryDCVoltageLevel=inData[17];

        DynamicJsonDocument root(256);
        //StaticJsonBuffer<256> jsonBuffer;
        //JsonObject& root = jsonBuffer.createObject();
        root["Timer_Loss"]=String(Timer_Loss);
        root["PowerTrouble"]=String(PowerTrouble);
        root["ACFailureTrouble"]=String(ACFailureTroubleIndicator);
        root["TelephoneLineTrouble"]=String(TelephoneLineTroubleIndicator);
        root["PSUDCVoltage"]=String(PowerSupplyDCVoltageLevel);
        root["BatteryDCVoltage"]=String(BatteryDCVoltageLevel);
        root["BatteryTrouble"]=String(NoLowBatteryTroubleIndicator);
        root["ACInputDCVoltageLevel"]=String(ACInputDCVoltageLevel);
        char output[256];
        
        serializeJson(root,output);
        //root.printTo(output);
        char topic[128];
        sprintf(topic,"%s/PanelStatus0",root_topicStatus);
        sendCharMQTT(topic,output ,false);  
    
    String Zonename ="";
    int zcnt = 0;
    
    
    sprintf(topic,"%s/ZoneStatus",root_topicStatus);
    for (int i = 19 ; i <= 22;i++)
    {
      DynamicJsonDocument zoneJson(256);
        strcpy(output,"");
        for (int j = 0 ; j < 8;j++) 
        {
            Zonename = "Z" + String(++zcnt);       
            zoneJson[Zonename] =  bitRead(inData[i],j);
        }       
        serializeJson(zoneJson,output);        
        sendCharMQTT(topic,output,false); 
        
    }
}



void createPabelstatus1Message()
{
  DynamicJsonDocument panelstatus1(256);
    char panelst[256];
     panelstatus1["Partition"]=_PanelStatus1msg.Partition;
        panelstatus1["Fire"]=_PanelStatus1msg.Fire;
        panelstatus1["Audible"]=_PanelStatus1msg.Audible;
        panelstatus1["Silent"]=_PanelStatus1msg.Silent;
        panelstatus1["AlarmFlg"]=_PanelStatus1msg.AlarmFlg;
        panelstatus1["StayFlg"]=_PanelStatus1msg.StayFlg;
        panelstatus1["SleepFlg"]=_PanelStatus1msg.SleepFlg;
        panelstatus1["ArmFlg"]=_PanelStatus1msg.ArmFlg;
        panelstatus1["zoneisbypassed"]=_PanelStatus1msg.zoneisbypassed;
      serializeJson(panelstatus1,panelst);
        char topic[128];
        sprintf(topic,"%s/%d/PanelStatus1",root_topicStatus,_PanelStatus1msg.Partition);
        sendCharMQTT(topic,panelst,false);  

        hassioStatus[_PanelStatus1msg.Partition].Partition=_PanelStatus1msg.Partition;
        homekitStatus[_PanelStatus1msg.Partition].Partition=_PanelStatus1msg.Partition;
      
       if (_PanelStatus1msg.AlarmFlg)
    {

       hassioStatus[_PanelStatus1msg.Partition].stringArmStatus="triggered";
       homekitStatus[_PanelStatus1msg.Partition].stringArmStatus="ALARM_TRIGGERED";
       homekitStatus[_PanelStatus1msg.Partition].intArmStatus=4;
    }
    else if (_PanelStatus1msg.StayFlg)
    {
       hassioStatus[_PanelStatus1msg.Partition].stringArmStatus="armed_home";
       homekitStatus[_PanelStatus1msg.Partition].stringArmStatus="STAY_ARM";
       homekitStatus[_PanelStatus1msg.Partition].intArmStatus=0;
    }else if (_PanelStatus1msg.SleepFlg)
    {
        hassioStatus[_PanelStatus1msg.Partition].stringArmStatus="armed_home";
       homekitStatus[_PanelStatus1msg.Partition].stringArmStatus="NIGHT_ARM";
       homekitStatus[_PanelStatus1msg.Partition].intArmStatus=2;
    }
    else if (_PanelStatus1msg.ArmFlg)
    {
        hassioStatus[_PanelStatus1msg.Partition].stringArmStatus = "armed_away";
         homekitStatus[_PanelStatus1msg.Partition].stringArmStatus = "AWAY_ARM";
         homekitStatus[_PanelStatus1msg.Partition].intArmStatus = 1;
    }
    else if (!_PanelStatus1msg.SleepFlg && !_PanelStatus1msg.StayFlg && !_PanelStatus1msg.ArmFlg)
    {
      if (TRACE)
        Debug.println("Disarmed set by createPabelstatus1Message");
        hassioStatus[_PanelStatus1msg.Partition].stringArmStatus = "disarmed";
        homekitStatus[_PanelStatus1msg.Partition].stringArmStatus = "DISARMED";
        homekitStatus[_PanelStatus1msg.Partition].intArmStatus = 3;
    }
    
    else
    {
        hassioStatus[_PanelStatus1msg.Partition].stringArmStatus = "unknown";
        homekitStatus[_PanelStatus1msg.Partition].stringArmStatus = "unknown";
        homekitStatus[_PanelStatus1msg.Partition].intArmStatus = 99;
    }

}


void PanelStatus1(){
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));


  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x01;
  data[33] = 0x05;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);
  trc(F("sending Panel Status 1 command to panel"));
  
  ParadoxSerial.write(data, MessageLength);
  
  readSerialQuick();

  
 _PanelStatus1msg.Partition=0;
  _PanelStatus1msg.Fire=bitRead(inData[17],7);
  _PanelStatus1msg.Audible=bitRead(inData[17],6);
  _PanelStatus1msg.Silent=bitRead(inData[17],5);
  _PanelStatus1msg.AlarmFlg=bitRead(inData[17],4);
  _PanelStatus1msg.StayFlg=bitRead(inData[17],2);
  _PanelStatus1msg.SleepFlg=bitRead(inData[17],1);
  _PanelStatus1msg.ArmFlg=bitRead(inData[17],0);
  _PanelStatus1msg.zoneisbypassed=bool(bitRead(inData[18],3));

  createPabelstatus1Message();
 _PanelStatus1msg.Partition=1;
  _PanelStatus1msg.Fire=bitRead(inData[21],7);
  _PanelStatus1msg.Audible=bitRead(inData[21],6);
  _PanelStatus1msg.Silent=bitRead(inData[21],5);
  _PanelStatus1msg.AlarmFlg=bitRead(inData[21],4);
  _PanelStatus1msg.StayFlg=bitRead(inData[21],2);
  _PanelStatus1msg.SleepFlg=bitRead(inData[21],1);
  _PanelStatus1msg.ArmFlg=bitRead(inData[21],0);
  _PanelStatus1msg.zoneisbypassed=bool(bitRead(inData[22],3));

  createPabelstatus1Message(); 
    
  sendArmStatus();
}



void doLogin(inPayload _inpayload){//byte pass1, byte pass2){
  byte data[MessageLength] = {};
  byte data1[MessageLength] = {};
  byte checksum;

  trc(F("Running doLogin Function"));

  memset(data, 0, sizeof(data));
  memset(data1, 0, sizeof(data1));
 
  //memset(data1, 0, sizeof(data1));

  
  data[0] = 0x5f;
  data[1] = 0x20;
  data[33] = 0x05;
  data[34] = 0x00;
  data[35] = 0x00;

  checksum = 0;

  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);
   trc(F("sending command 0x5f to panel"));
   if (TRACE)
      {
        Debug.println("DOLOGIN: DATA");
        for (int i = 0; i < sizeof(data1); i++) Debug.println(data[i], HEX);
        Debug.println();
      }

    ParadoxSerial.write(data, MessageLength);
    sleep(1);
    readSerialQuick();

    
    trc(F("got callback from 0x5f command"));
      data1[0] = 0x00;
      data1[4] = inData[4];
      data1[5] = inData[5];
      data1[6] = inData[6];
      data1[7] = inData[7];
      data1[7] = inData[8];
      data1[9] = inData[9];
      //data1[10] = pass1; //panel pc password digit 1 & 2
      //data1[11] = pass2; //panel pc password digit 3 & 4
      data1[10] = 0x00;
      data1[11] = 0x00;
      data1[13] = 0x55;
      data1[14] = _inpayload.PcPasswordFirst2Digits; //panel pc password digit 1 & 2
      data1[15] = _inpayload.PcPasswordSecond2Digits; //panel pc password digit 3 & 4
      if (USE6DigitCode)
      data1[16] = _inpayload.PcPasswordThird2Digits; //panel pc password digit 3 & 4

      data1[33] = 0x05;

     
      checksum = 0;
      for (int x = 0; x < MessageLength - 1; x++)
      {
        checksum += data1[x];
      }
      data1[36] = checksumCalculate(checksum);
      
      trc("sending command 0x00 to panel");
      if (TRACE)
      {
        Debug.println("DOLOGIN: DATA1");
        for (int i = 0; i < sizeof(data1); i++) Debug.println(data1[i], HEX);
        Debug.println();
      }

      ParadoxSerial.write(data1, MessageLength);  
      sleep(1);
           
      readSerialQuick();
      if ((inData[0] & 0xF0) == 0x10)
      {
        PanelConnected = true;
        trc(F("Panel login success"));
      }else{
        trc(F("Panel login failed"));
      }
      
      
      trc(F("Panel login complete"));
}

struct inPayload Decodejson(char *Payload){
  inPayload indata;
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject &root = jsonBuffer.parseObject(Payload);
  DynamicJsonDocument root(256);
  auto deserializeError = deserializeJson(root,Payload,256);

  if (deserializeError)
  {
    indata = {0x00,0x00,0x00,0x00};
    trc("JSON parsing failed!");
    JsonParseError=true;
    return indata;
  }
  else
  {
    char charpass1[4];
    char charpass2[4];
    
    char charsubcommand[4];
   
    String command="";
    String subcommand="";
    String password = root["password"];
    
    
    if (root.containsKey("Subcommand"))
    {
      subcommand = root["Subcommand"].as<String>();
    }else
    {
      subcommand = "0";
    }
    
    if (root.containsKey("Command"))
    {
       command = root["Command"].as<String>();
    }
    
    if (root.containsKey("SecuritySystemTargetState"))
    {
        command=root["SecuritySystemTargetState"].as<String>();
    }

    String pass1 = password.substring(0, 2);
    String pass2 = password.substring(2, 4);

    byte PanelPassword3=0xFF;
    if (USE6DigitCode)
    {
      char charpass3[4];
      String pass3 = password.substring(4, 6);
      pass3.toCharArray(charpass3, 4);
      unsigned long passNumber3 = strtoul(charpass3, NULL, 16);
      if (passNumber3 < 10)
        passNumber3 = passNumber3 + 160;
      PanelPassword3 = passNumber3 & 0xFF; 
    }

    // trc(pass1);
    // trc(pass2);

    pass1.toCharArray(charpass1, 4);
    pass2.toCharArray(charpass2, 4);
    subcommand.toCharArray(charsubcommand,4);

    unsigned long number1 = strtoul(charpass1, NULL, 16);
    unsigned long number2 = strtoul(charpass2, NULL, 16);
    unsigned long number3 = strtoul(charsubcommand, NULL, 0);
    
    


    if (number2 < 10)
     number2 = number2 + 160;

    if (number1 < 10)
      number1 = number1 + 160;

   

    byte PanelPassword1 = number1 & 0xFF; 
    byte PanelPassword2 = number2 & 0xFF; 
    byte SubCommand = number3 & 0xFF;

if (TRACE)
{
    Debug.println("*******************");
    Debug.println(charpass1);
    Debug.println(charpass2);
    Debug.println(number1);
    Debug.println(number2);
    
    Debug.println(PanelPassword1,HEX);
    Debug.println(PanelPassword2,HEX);
}

    byte CommandB = getPanelCommand(command) ;
  
    inPayload data1 = {PanelPassword1, PanelPassword2, PanelPassword3, CommandB, SubCommand};

    return data1;
  }
  return indata;
}

void serial_flush_buffer(){
  trc("starting serial flush");
  ParadoxSerial.flush();
  delay(1000);
  
  while (ParadoxSerial.read() >= 0)
  {
    trc("flushing........");
  }
  trc("serial clean");
}


boolean reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    trc("Attempting MQTT connection...");
    String mqname =  WiFi.macAddress();
    char charBuf[50];
    mqname.toCharArray(charBuf, 50) ;
    
    if (WiFi.status() != WL_CONNECTED) {
      //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      //Serial.println("Connection Failed! Rebooting...");
      //delay(5000);
      //ESP.restart();
      //}
      WiFi.begin();
      vTaskDelay(5000);
    }
   
   

    if (client.connect(charBuf,Mqtt_Username,Mqtt_Password,LWT,2,true,"OFF")) {
    // Once connected, publish an announcement...
      //client.publish(root_topicOut,"connected");
      trc("MQTT connected");
      sendMQTT(LWT, "ON", true);
      //Topic subscribed so as to get data
      char topicNameRec[50] ;
      sprintf(topicNameRec, "%s/%s", Hostname, def_topicIn);
      //Subscribing to topic(s)
      subscribing(topicNameRec);
    } else {
      trc("failed, rc=");
      trc(String(client.state()));
      trc(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return client.connected();
}

void setup_wifi(){
  
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", Mqtt_ServerAddress, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", Mqtt_ServerPort, 6);

     WiFiManagerParameter custom_MqttUserName("Username", "mqtt Username", Mqtt_Username, 40);
     WiFiManagerParameter custom_MqttUserPassword("Password", "mqtt Password", Mqtt_Password, 40);

    WiFiManager wifiManager;
    if (ResetConfig)
    {
      trc(F("Resetting wifiManager"));
      WiFi.disconnect();
      wifiManager.resetSettings();
    }
       
    if (strcmp(Mqtt_ServerPort,"")== 0  || strcmp(Mqtt_ServerAddress,"")== 0)
    {
      trc(F("Resetting wifiManager"));
      WiFi.disconnect();
      wifiManager.resetSettings();
      ESP.restart();
      delay(1000);
    }
    else
    {
      trc(F("values ar no null "));
    }


    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.setConfigPortalTimeout(180);
    
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_MqttUserName);
    wifiManager.addParameter(&custom_MqttUserPassword);
        
    if (!wifiManager.autoConnect(Hostname,"configParadox32")) {
      trc(F("failed to connect and hit timeout"));
      digitalWrite(LED_BUILTIN,HIGH);
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    trc(F("connected...yeey :)"));
  
    strcpy(Mqtt_ServerAddress, custom_mqtt_server.getValue());
    strcpy(Mqtt_ServerPort, custom_mqtt_port.getValue());
    strcpy(Mqtt_Username, custom_MqttUserName.getValue());
    strcpy(Mqtt_Password, custom_MqttUserPassword.getValue());


    
    //save the custom parameters to FS
    if (shouldSaveConfig) {
      trc(F("saving config"));
      //DynamicJsonBuffer jsonBuffer;
      //JsonObject& json = jsonBuffer.createObject();
      DynamicJsonDocument json(256);

      json["mqtt_server"] = Mqtt_ServerAddress;
      json["mqtt_port"] = Mqtt_ServerPort;
      json["mqtt_user"] = Mqtt_Username;
      json["mqtt_password"] = Mqtt_Password;
      
      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        trc(F("failed to open config file for writing"));
      }
  
      serializeJson(json,ParadoxSerial);
      serializeJson(json,configFile);
      configFile.close();
      //end save
    }
  
    trc(F("Setting Mqtt Server values"));
    trc(F("mqtt_server : "));
    trc(mqtt_server);
    trc(F("mqtt_server_port : "));
    trc(mqtt_port);

    trc(F("Setting Mqtt Server connection"));
    unsigned int mqtt_port_x = atoi (mqtt_port); 
    client.setServer(mqtt_server, mqtt_port_x);
    
    client.setCallback(callback);
   
    reconnect();
    
    
    Debug.println(F("WiFi connected"));
    Debug.print("IP address:");
    Debug.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN,HIGH);
  
}






void mountfs(){
   if (SPIFFS.begin(true)) {
    trc(F("mounted file system"));
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      trc(F("reading config file"));
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        trc(F("opened config file"));
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        //DynamicJsonBuffer jsonBuffer;
        //JsonObject& json = jsonBuffer.parseObject(buf.get());
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get(), size);

        serializeJson(json,ParadoxSerial);
        if (!deserializeError) {
          trc(F("\nparsed json"));

          strcpy(Mqtt_ServerAddress, json["mqtt_server"]);
          strcpy(Mqtt_ServerPort , json["mqtt_port"]);
          strcpy(Mqtt_Username, json["mqtt_user"]);
          strcpy(Mqtt_Password, json["mqtt_password"]);
          
        } else {
          trc(F("failed to load json config"));
          
        }
      }
    }
    else
    {
      trc(F("File /config.json doesnt exist"));
      //SPIFFS.format();
      trc(F("Formatted Spiffs"));    
    }
  } else {
    trc(F("failed to mount FS"));
  }
}



void setup() {

  


  SetupMqttServer();
  SetupMqttTopics();
  pinMode(LED, OUTPUT);
  
  WiFi.mode(WIFI_STA);

  
  ParadoxSerial.begin(9600 ,SERIAL_8N1);
  
  Debug.begin(9600,SERIAL_8N1);

  

  ParadoxSerial.flush(); // Clean up the serial buffer in case previous junk is there
  GSMModule.flush();
  Debug.flush();
  Debug.println(firmware);
  
  trc(F("serial monitor is up"));
  serial_flush_buffer();

  

  trc(F("Running MountFs"));
  mountfs();

  setup_wifi();
  StartSSDP();

  if (WiFi.status() == WL_CONNECTED) {
      WiFi.setHostname(Hostname);
   }
  
  ArduinoOTA.setHostname(Hostname);
  ArduinoOTA.setTimeout(60*1000);
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Debug.println("Start updating " + type);
    })
    .onEnd([]() {
      Debug.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      //Debug.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Debug.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Debug.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Debug.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Debug.println("Receive Failed");
      else if (error == OTA_END_ERROR) Debug.println("End Failed");
    });

  ArduinoOTA.begin();
  trc("Finnished wifi setup");
  delay(1500);
  
  char readymsg[64];
  sprintf(readymsg, " {\"firmware\":\"SYSTEM %s\"} ", firmware);
  sendCharMQTT(root_topicStatus, readymsg, false);
  lastReconnectAttempt = 0;
  serial_flush_buffer();
  configTime(gmtOffset_sec, 0, ntpServer);

  if (ArmStateRefresh>0)
   timer.setInterval(ArmStateRefresh*1000, sendArmStatus);
  
  
  sendMQTT(root_topicStatus, "{\"status\":\"System Ready\"}" , false);
    
}

void loop() {

  if (!(ParadoxSerial.available()<37))
   readSerial();  
   
   if ( (inData[0] & 0xF0) != 0xE0 && (inData[0] & 0xF0) != 0x40 && (inData[0] & 0xF0) != 0x50 && (inData[0] & 0xF0) != 0x30 && (inData[0] & 0xF0) != 0x70)
    {
      trc(F("start serial_flush_buffer"));
      memset(inData,0,sizeof(inData));
      serial_flush_buffer(); 
      
    }
    if (OTAUpdate==1)
    {
      ArduinoOTA.handle();
    }
     
    HTTP.handleClient();
    handleMqttKeepAlive();
    if (ArmStateRefresh>0)
      timer.run();
    
  
}


String getpage(){
String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += F("<dl>");
  page += F("<dt>Hostname</dt><dd>");
  page += Hostname;
  page += F("</dd>");
  page += F("<dt>Firmware</dt><dd>");
  page += F(firmware);
  page += F("</dd>");
  page += F("<dt>Hassio</dt><dd>");
  page += Hassio;
  page += F("</dd>");

   page += F("<dt>Homekit</dt><dd>");
  page += HomeKit;
  page += F("</dd>");
  
  page += F("<dt>Chip ID</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  // page += F("<dt>Flash Chip ID</dt><dd>");
  // page += ESP.getFlashChipId();
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  // page += F("<dt>Real Flash Size</dt><dd>");
  // page += ESP.getFlashChipRealSize();
  page += F(" bytes</dd>");

  page += F("<dt>Local IP</dt><dd>");
  page += WiFi.localIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");

  page += F("<dt>root_topicIn</dt><dd>");
  page += root_topicIn;
  page += F("</dd>");

   page += F("<dt>root_topicOut</dt><dd>");
  page += root_topicOut;
  page += F("</dd>");

   page += F("<dt>root_topicHassioArm</dt><dd>");
  page += root_topicHassioArm;
  page += F("</dd>");

     page += F("<dt>root_topicHassio</dt><dd>");
  page += root_topicHassio;
  page += F("</dd>");

   page += F("<dt>root_topicArmHomekit</dt><dd>");
  page += root_topicArmHomekit;
  page += F("</dd>");

  page += F("</dl>");
  
return page;

}
 
