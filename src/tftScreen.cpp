#include "tftScreen.h"


 
tftScreen::tftScreen(bool enabled)
{

  isEnabled = enabled;
   TFT_eSPI tft = TFT_eSPI(135, 240);
}

tftScreen::~tftScreen()
{

}

void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void tftScreen::SetupTFT()
{
  
     tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

  tft.setSwapBytes(true);
  tft.setRotation(3);
    tft.fillScreen(TFT_RED);
    espDelay(1000);
    tft.fillScreen(TFT_BLUE);
    espDelay(1000);
    tft.fillScreen(TFT_GREEN);
    espDelay(1000);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.setTextDatum(TL_DATUM);
    tft.println("ParadoxMQTT");
    tft.println("Starting");
}

 void tftScreen::printTFT(char* Message){

     if (isEnabled)
     {
         tft.println("Message");
     }


 }

 void tftScreen::ClearScreen(){

     if (isEnabled)
     {
         tft.fillScreen(TFT_BLACK);
         tft.setCursor(0,0);
     }


 }

void tftScreen::writeTopMiddle(char* message)
{
  if(isEnabled)
  {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(0, 0);
  tft.drawString(message, 0, tft.height() / 2);
  }
}






