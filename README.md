# ParadoxMqtt32

This project uses an ESP32 to read the events of the serial bus on Paradox alarm systems and send them to an MQTT server

## Making a connection

Connect the panel through serial connection:
- Alarm system serial to ESP32 using RX2/TX2 of ESP32<br>


## Arduino IDE settings

Set the _Hassio_ flag to _1_ for Home assistant, and check out the wiki (Home Assistant in V2)
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group", and one more dummy attribute which is the zone/partition label.

See the wiki for more info on Groups and Sub-groups

After flashing the ESP board, connect to it's Wi-Fi (_paradox32CTL_), open the 192.168.4.1 IP address in your browser, input your Wi-Fi credentials and MQTT server address. That's all.  

### MQTT Topics 

| Topic              | Notes                     |
|--------------------|---------------------------|
| paradox32CTL/out    | All alarm event messages  |
| paradox32CTL/status | The program messages      |
| paradox32CTL/in     | Input topic               |

### HomeAssistant MQTT Topics

| Topic                        | Notes                                                     |
|------------------------------|-----------------------------------------------------------|
| paradox32CTL/hassio/zoneX | Where x is zone number from 1-32                          |
| paradox32CTL/hassio/zoneX | Gives values ON/OFF                                       |
| paradox32CTL/hassio/Arm       | Gives values: disarmed, armed_home, armed_away, triggered |

### Sending commands

The command payloads are in JSON. Template:
```json
{
 "password":"1234",
 "Command":"arm",
 "Subcommand":"0"
}
```
The password is the user's 4 digit password.

A command can be any of the following:
- arm
- disarm
- sleep
- stay
- bypass
- armstate
- panelstatus
- setdate
- PGM_ON
- PGM_OFF
	
#### Subcommands depending on the main command
	
| Main Command     | Subcommand                     |
|------------------|--------------------------------|
| arm,sleep,disarm | partition                      |
| bypass           | The zone number from 0 to 31   |
| panelstatus      | panel data                     |
| panelstatus      | panel voltage and battery data |

### Release Logs

20200126: 
1. Added ArmStatus: pending when exit delay 
2. Added Event 29 (because of change pending status to arm_away after exit delay) 

20190212:
- Added retain message on hassio/Arm topic<br>
- Added the ability to add credentials to mqtt.<br>
- Added Homekit topic for Homebridge plugin. (comming soon). <br>	
	
20190130: added PGM support (command "PGM_ON" subcomand "0-31)
  
20190114: V2 Live (Homeassistant)

20190104: Added wiki Node-red v2 flow 

20190103: Added v2 test branch (stable working) 

20180804: Wiki added Home Assistant Config (works with node-red) 

20180721: Changed to user based password, use the same 4 digit code used on panel for control. 



Continue reading the wiki for more information.
