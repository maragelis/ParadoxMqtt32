

#define homebridgeRootTopic "homebridgeParadox"
#define HomekitDeviceNameP0 "Paradox_Alarm_P0"
#define HomekitDeviceNameP1 "Paradox_Alarm_P1"
#define HB_toAdd "to/add"
#define HB_addService "to/add/service"
#define HB_toRemove "to/remove"
#define HB_removeService "to/remove/service"
#define HB_toGet "to/get"
#define HB_toSet "to/set"
#define HB_setReachability "to/set/reachability"
#define HB_setAccessoryinformation "to/set/accessoryinformation"
#define HB_fromGet "from/get"
#define HB_fromSet "from/set"
#define HB_fromResponse "from/response"
#define HB_fromIdentify "from/identify"

enum SecuritySystemCurrentStates
{
    STAY_ARM=0,AWAY_ARM = 1,NIGHT_ARM = 2,DISARMED = 3,ALARM_TRIGGERED = 4
};

enum SecuritySystemStates
{
    SecuritySystemTargetState=0,SecuritySystemCurrentState=1
};



