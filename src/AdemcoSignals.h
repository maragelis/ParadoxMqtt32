#ifndef AdemcoSignals_h
#define AdemcoSignals_h


#include "Arduino.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <string>
#include <iostream>

// Please select the corresponding model

 #define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609


// #define TEST_RING_RI_PIN            //Note will cancel the phone call test

// #define ENABLE_SPI_SDCARD   //Uncomment will test external SD card

// Define the serial console for debug prints, if needed
// #define DUMP_AT_COMMANDS
// #define TINY_GSM_DEBUG SerialMon

#include "GsmUtilities.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800          // Modem is SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set RX buffer to 1Kb

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);

#endif

TinyGsmClient tinyGsmClient(modem);


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */

SPIClass SPI1(HSPI);

#define MY_CS       33
#define MY_SCLK     25
#define MY_MISO     27
#define MY_MOSI     26

enum eventqualifier
{
    NewEvent_or_Opening=1,
    NewRestore_or_Closing=3,
    Previously_reported_condition_still_present=6

};


struct AlarmMessage
{
    char* PhoneNumber;
    char* AccountNumber;
    char* MessageType; //18
    eventqualifier EventQualifier;
    char*  AlarmPartition;
    char* ZoneNumber;
    char* checksum;

};



class AdemcoSignals
{
private:
    bool CallAnswered = false;
    bool res = false;
    
public:

    
    AdemcoSignals(/* args */);
    ~AdemcoSignals();
    void SetupAdemco();  
    void setupModem();
    void turnOffNetlight();
    void turnOnNetlight();
    void HangUpCall();
    void AdemcoCallNumber(char* Number);
    void SendDTMF(char Tone, int duration);
    void SendDTMFsequense(char* chars);
    bool SendMessage(AlarmMessage msg);
    AlarmMessage CalculateChecksum(AlarmMessage alarmMessage);
    
   
    
};



AdemcoSignals::~AdemcoSignals()
{
}




enum ademcoSignalenum
{
//Medical Alarms –100
Ademco_Medical_Alarm=100,
Ademco_Personal_Emergency=101,
Ademco_Fail_to_report_in=102,

//Fire Alarms –110
Ademco_Fire=110,
Ademco_Smoke=111,
Ademco_Combustion=112,
Ademco_Water_flow=113,
Ademco_Heat=114,
Ademco_Pull_Station=115,
Ademco_Duct=116,
Ademco_Flame=117,
Ademco_Near_Alarm=118,

//Panic Alarms –120
Ademco_Panic=120,
Ademco_Duress=121,
Ademco_Silent=122,
Ademco_Audible=123,
Ademco_Duress_Access_granted=124,
Ademco_Duress_Egress_granted=125,

//Burglar Alarms –130
Ademco_Burglary=130,
Ademco_Perimeter=131,
Ademco_Interior=132,
Ademco_24_Hour_Safe=133,
Ademco_Entry_Exit=134,
Ademco_Day_night=135,
Ademco_Outdoor=136,
Ademco_Tamper=137,
Ademco_Near_alarm=138,
Ademco_Intrusion_Verifier=139,

//General Alarm – 140
Ademco_General_Alarm=140,
Ademco_Polling_loop_open=141,
Ademco_Polling_loop_short=142,
Ademco_Expansion_module_failure=143,
Ademco_Sensor_tamper=144,
Ademco_Expansion_module_tamper=145,
Ademco_Silent_Burglary=146,
Ademco_Sensor_Supervision_Failure=147,

//24 Hour Non-Burglary - 150 and 160
Ademco_24_Hour_Non_Burglary=150,
Ademco_Gas_detected=151,
Ademco_Refrigeration=152,
Ademco_Loss_of_heat=153,
Ademco_Water_Leakage=154,
Ademco_Foil_Break=155,
Ademco_Day_Trouble=156,
Ademco_Low_bottled_gas_level=157,
Ademco_High_temp=158,
Ademco_Low_temp=159,
Ademco_Loss_of_air_flow=161,
Ademco_Carbon_Monoxide_detected=162,
Ademco_Tank_level=163,
Ademco_High_Reading_Alarm=168,
Ademco_Low_Reading_Alarm=169,

//SUPERVISORY
//Fire Supervisory - 200 and 210
Ademco_Fire_Supervisory=200,
Ademco_Low_water_pressure=201,
Ademco_Low_CO2=202,
Ademco_Gate_valve_sensor=203,
Ademco_Low_water_level=204,
Ademco_Pump_activated=205,
Ademco_Pump_failure=206,

//TROUBLES
//System Troubles -300 and 310
Ademco_System_Trouble=300,
Ademco_AC_Loss=301,
Ademco_Low_system_battery=302,
Ademco_RAM_Checksum_bad=303,
Ademco_ROM_checksum_bad=304,
Ademco_System_reset=305,
Ademco_Panel_programming_changed=306,
Ademco_Self_test_failure=307,
Ademco_System_shutdown=308,
Ademco_Battery_test_failure=309,
Ademco_Ground_fault=310,
Ademco_Battery_Missing_Dead=311,
Ademco_Power_Supply_Overcurrent=312,
Ademco_Engineer_Reset=313,

//Sounder / Relay Troubles -320
Ademco_Sounder_Relay=320,
Ademco_Bell_1=321,
Ademco_Bell_2=322,
Ademco_Alarm_relay=323,
Ademco_Trouble_relay=324,
Ademco_Reversing_relay=325,
Ademco_Notification_Appliance_Ckt_3=326,
Ademco_Notification_Appliance_Ckt_4=327,


//System Peripheral Trouble -330 and 340
Ademco_System_Peripheral_trouble=330,
Ademco_System_Peripheral_Polling_loop_open=331,
Ademco_System_Peripheral_Polling_loop_short=332,
Ademco_System_Peripheral_Expansion_module_failure=333,
Ademco_Repeater_failure=334,
Ademco_Local_printer_out_of_paper=335,
Ademco_Local_printer_failure=336,
Ademco_Exp_Module_DC_Loss=337,
Ademco_Exp_Module_Low_Batt=338,
Ademco_Exp_Module_Reset=339,
Ademco_Exp_Module_Tamper=341,
Ademco_Exp_Module_AC_Loss=342,
Ademco_Exp_Module_selftest_fail=343,
Ademco_RF_Receiver_Jam_Detect=344,

//Communication Troubles -350 and 360
Ademco_Communication_trouble=350,
Ademco_Telco_1_fault=351,
Ademco_Telco_2_fault=352,
Ademco_Long_Range_Radio_xmitter_fault=353,
Ademco_Failure_to_communicate_event=354,
Ademco_Loss_of_Radio_supervision=355,
Ademco_Loss_of_central_polling=356,
Ademco_Long_Range_Radio_VSWR_problem=357,

//Protection Loop -370
Ademco_Protection_loop=370,
Ademco_Protection_loop_open=371,
Ademco_Protection_loop_short=372,
Ademco_Fire_trouble=373,
Ademco_Exit_error_alarm_zone=374,
Ademco_Panic_zone_trouble=375,
Ademco_Hold_up_zone_trouble=376,
Ademco_Swinger_Trouble=377,
Ademco_Cross_zone_Trouble=378,

//Sensor Trouble -380
Ademco_Sensor_trouble=380,
Ademco_Loss_of_supervision_RF=381,
Ademco_Loss_of_supervision_RPM=382,
Ademco_Trouble_Sensor_tamper=383,
Ademco_RF_low_battery=384,
Ademco_Smoke_detector_Hi_sensitivity=385,
Ademco_Smoke_detector_Low_sensitivity=386,
Ademco_Intrusion_detector_Hi_sensitivity=387,
Ademco_Intrusion_detector_Low_sensitivity=388,
Ademco_Sensor_selftest_failure=389,
Ademco_Sensor_Watch_trouble=391,
Ademco_Drift_Compensation_Error=392,
Ademco_Maintenance_Alert=393,

//OPEN/CLOSE/REMOTE ACCESS
//Open/Close -400, 440,450
Ademco_Open_Close=400,
Ademco_OC_by_user=401,
Ademco_Group_OC=402,
Ademco_Automatic_OC=403,
Ademco_Late_to_OC__Note_use_453_instead404=404,
Ademco_Deferred_OC__Obsolete_do_not_use405=405,
Ademco_Cancel=406,
Ademco_Remote_arm_disarm=407,
Ademco_Quick_arm=408,
Ademco_Keyswitch_OC=409,

//Remote Access –410
Ademco_Callback_request_made=411,
Ademco_Successful_downloadaccess=412,
Ademco_Unsuccessful_access=413,
Ademco_System_shutdown_command_received=414,
Ademco_Dialer_shutdown_command_received=415,
Ademco_Successful_Upload=416,

//Access control –420,430
Ademco_Access_denied=421,
Ademco_Access_report_by_user=422,
Ademco_Forced_Access=423,
Ademco_Egress_Denied=424,
Ademco_Egress_Granted=425,
Ademco_Access_Door_propped_open=426,
Ademco_Access_point_Door_Status_Monitor_trouble=427,
Ademco_Access_point_Request_To_Exit_trouble=428,
Ademco_Access_program_mode_entry=429,
Ademco_Access_program_mode_exit=430,
Ademco_Access_threat_level_change=431,
Ademco_Access_relaytrigger_fail=432,
Ademco_Access_RTE_shunt=433,
Ademco_Access_DSM_shunt=434,


Ademco_Armed_STAY=441,
Ademco_Keyswitch_Armed_STAY=442,
Ademco_Exception_OC=450,
Ademco_Early_OC=451,
Ademco_Late_OC=452,
Ademco_Failed_to_Open=453,
Ademco_Failed_to_Close=454,
Ademco_Autoarm_Failed=455,
Ademco_Partial_Arm=456,
Ademco_Exit_Error_user=457,
Ademco_User_on_Premises=458,
Ademco_Recent_Close=459,
Ademco_Wrong_Code_Entry=461,
Ademco_Legal_Code_Entry=462,
Ademco_Rearm_after_Alarm=463,
Ademco_Autoarm_Time_Extended=464,
Ademco_Panic_Alarm_Reset=465,
Ademco_Service_OnOff_Premises=466,


//BYPASSES / DISABLES
//System Disables -500 and 510
Ademco_Access_reader_disable=501,

//Sounder / Relay Disables -520
Ademco_SounderRelay_Disable=520,
Ademco_Bell_1_disable=521,
Ademco_Bell_2_disable=522,
Ademco_Alarm_relay_disable=523,
Ademco_Trouble_relay_disable=524,
Ademco_Reversing_relay_disable=525,
Ademco_Notification_Appliance_Ckt_3_disable=526,
Ademco_Notification_Appliance_Ckt_4_disable=527,

//System Peripheral Disables -530 and 540
Ademco_Module_Added=531,
Ademco_Module_Removed=532,

//CommunicationDisables -550 and 560
Ademco_Dialer_disabled=551,
Ademco_Radio_transmitter_disabled=552,
Ademco_Remote_UploadDownload_disabled=553,

//Bypasses –570
Ademco_ZoneSensor_bypass=570,
Ademco_Fire_bypass=571,
Ademco_24_Hour_zone_bypass=572,
Ademco_Burg_Bypass=573,
Ademco_Group_bypass=574,
Ademco_Swinger_bypass=575,
Ademco_Access_zone_shunt=576,
Ademco_Access_point_bypass=577,

//TEST / MISC.
//Test/Misc. –600, 610
Ademco_Manual_trigger_test_report=601,
Ademco_Periodic_test_report=602,
Ademco_Periodic_RF_transmission=603,
Ademco_Fire_test=604,
Ademco_Status_report_to_follow=605,
Ademco_Listen_in_to_follow=606,
Ademco_Walk_test_mode=607,
Ademco_Periodic_test_System_Trouble_Present=608,
Ademco_Video_Xmitter_active=609,
Ademco_Point_tested_OK=611,
Ademco_Point_not_tested=612,
Ademco_Intrusion_Zone_Walk_Tested=613,
Ademco_Fire_Zone_Walk_Tested=614,
Ademco_Panic_Zone_Walk_Tested=615,
Ademco_Service_Request=616,

//Event Log –620
Ademco_Event_Log_reset=621,
Ademco_Event_Log_50_full=622,
Ademco_Event_Log_90_full=623,
Ademco_Event_Log_overflow=624,
Ademco_TimeDate_reset=625,
Ademco_TimeDate_inaccurate=626,
Ademco_Program_mode_entry=627,
Ademco_Program_mode_exit=628,
Ademco_32_Hour_Event_log_marker=629,

//Scheduling –630
Ademco_Schedule_change=630,
Ademco_Exception_schedule_change=631,
Ademco_Access_schedule_change=632,

//Personnel Monitoring -640
Ademco_Senior_Watch_Trouble=641,
Ademco_Latch_Key_Supervision=642,

//Misc. -650
Ademco_Reserved_for_Ademco_Use1=651,
Ademco_Reserved_for_Ademco_Use2=652,
Ademco_Reserved_for_Ademco_Use3=653,
Ademco_System_Inactivity=654
};





#endif