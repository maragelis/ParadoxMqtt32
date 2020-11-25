#include "AdemcoSignals.h"





AdemcoSignals::AdemcoSignals(/* args */)
{
     
    

}

void ademcoDelay(int time)
{
    int period = time;
    unsigned long time_now = 0;
    time_now = millis();
          
    while(millis() < time_now + period){
        ;
    }
}

bool AdemcoSignals::SendMessage(AlarmMessage msg)
{
    msg = CalculateChecksum(msg);
    bool retval = false;
    int wait =0;
    AdemcoCallNumber(msg.PhoneNumber);
    
    while (!CallAnswered)
    {
        ademcoDelay(1000);
        wait++;
        if(wait>10)
            return retval;
    }

    

    //Burst     
     //E represents single frequency 1400HZ sound, F represents single frequency 2300HZ sound. 
    SendDTMF('E',100);
    SendDTMF('F',100);
    


    return retval;

}



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


void AdemcoSignals::AdemcoCallNumber(char* Number)
{
    String s = Number;
    res = modem.callNumber(s);
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
    //char cars[6] = "Volvo";
    for(int i = 0;  s[i]!='\0'; i++) {
    //printf("%c \n",cars[i]) ;
        chk+=ConvertChar2Int(s[i]);
    }
    return chk;
}

AlarmMessage AdemcoSignals::CalculateChecksum(AlarmMessage alarmMessage)
{
    int chksum =0;
    int sum = 0;

    char checksum[2 + sizeof(char)];
    sum += getSumOfChar(alarmMessage.AccountNumber);
    sum += getSumOfChar(alarmMessage.MessageType);
    sum += (int)alarmMessage.EventQualifier;
    sum += getSumOfChar(alarmMessage.ZoneNumber);
    
    //Find the next highest multiple of 15
    for(int i = 15; i<sum +15 ; i=i+15) {
        chksum = i;
    }
    chksum =chksum-sum;

    if (chksum==0)
    {
        chksum=15;
    }
    sprintf(checksum, "%d", chksum);
    alarmMessage.checksum = checksum;

    return alarmMessage;
}

void AdemcoSignals::SendDTMF(char tone, int duration)
{
    modem.dtmfSend(tone,duration);
}
void AdemcoSignals::SendDTMFsequense(char* chars)
{
    int digit = 0;
    for(int i = 0;  chars[i]!='\0'; i++) {
    //printf("%c \n",cars[i]) ;
        digit=ConvertChar2Int(chars[i]);
    }

    modem.dtmfSend(digit);
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




