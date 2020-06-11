#ifndef tftScreen_H
#define tftScreen_H

#include <TFT_eSPI.h>
#include <Arduino.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4   // Display backlight control pin
#define ADC_EN          14  //ADC_EN is the ADC detection enable port


class tftScreen
{
private:
    bool isEnabled;
    
public:
   TFT_eSPI tft;
    tftScreen(bool enabled);
    ~tftScreen();

    void SetupTFT();
    void writeTopMiddle(char* message);
    void ClearScreen();
    
    void printTFT(char* Message);

};




#endif