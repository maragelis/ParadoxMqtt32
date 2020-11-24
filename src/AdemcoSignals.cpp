#include "AdemcoSignals.h"




struct AlarmMessage
{
    char AccountNumber[4];
    char MessageType[2];
    eventqualifier EventQualifier;
    char  Partition[3];
    char ZoneNumber[2];
    char checksum[2];

};






bool CallAnswered = false;
bool res = false;

void AdemcoSignals::setupModem()
{
#ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif

    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    // Turn on the Modem power first
    digitalWrite(MODEM_POWER_ON, HIGH);

    // Pull down PWRKEY for more than 1 second according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);

    // Initialize the indicator as an output
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LED_OFF);
}


void AdemcoSignals::CallNumber(String Number)
{
    res = modem.callNumber(Number);
    if (res)
    {
        delay(1000L);
        CallAnswered = true;
    }
    else
    {
        CallAnswered = false;
    }
    
}
int ConvertChar2Int(char character )
{
    
    int x = character - '0';
    return x;
}
int getSumOfChar(char* s)
{
    int chk = 0;
    char * t; 
    for (t = s; s != '\0'; t++) {
        chk += ConvertChar2Int(s[t]);
    }
}

AlarmMessage AdemcoSignals::CalculateChecksum(AlarmMessage alarmMessage)
{
    int chksum =60;
    for (t = s; s != '\0'; t++) {
        size++;
    }
}

void AdemcoSignals::SendDTMF(char tone, int duration)
{
    modem.dtmfSend(tone,duration);
}



void AdemcoSignals::HangUpCall()
{
    res = modem.callHangup();
    if (res)
    {
        CallAnswered = false;
    }
}

void AdemcoSignals::turnOffNetlight()
{
    SerialMon.println("Turning off SIM800 Red LED...");
    modem.sendAT("+CNETLIGHT=0");
}

void AdemcoSignals::turnOnNetlight()
{
    SerialMon.println("Turning on SIM800 Red LED...");
    modem.sendAT("+CNETLIGHT=1");
}


void AdemcoSignals::SetupAdemco()
{
    
    

    
}




