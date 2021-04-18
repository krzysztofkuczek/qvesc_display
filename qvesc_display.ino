// author: Krzysztof Kuczek
// https://qczek.beyondrc.com
// check how it works https://www.youtube.com/watch?v=XjOzGHkT_UA
// please note that this is only quick and dirty project to utilize ttgo t board and cheap HC-06 bt for VESC parameters monitoring
// it use free for non comercial usse fonts, ttgo sample project files and modified VescUart library  
// to make it work you need to define your bt address and pin, check vesc_bt_address in bt.cpp file


#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include "Button2.h"
#include "esp_adc_cal.h"
#include "bmp.h"
#include "bt.h"
#include "dashboard.h"



// TFT Pins has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
// #define TFT_MOSI            19
// #define TFT_SCLK            18
// #define TFT_CS              5
// #define TFT_DC              16
// #define TFT_RST             23
// #define TFT_BL              4   // Display backlight control pin


#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_LEFT            0
#define BUTTON_RIGHT            35

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Dashboard dbrd;
bool btConnected = false;
int64_t lastDisplayUpdateTime = 0;


Button2 btnLeft(BUTTON_LEFT);
Button2 btnRight(BUTTON_RIGHT);

char buff[512];
int vref = 1100;
int btnCick = false;

//#define ENABLE_SPI_SDCARD

//Uncomment will use SDCard, this is just a demonstration,
//how to use the second SPI
#ifdef ENABLE_SPI_SDCARD

#include "FS.h"
#include "SD.h"

SPIClass SDSPI(HSPI);

#define MY_CS       33
#define MY_SCLK     25
#define MY_MISO     27
#define MY_MOSI     26

void setupSDCard()
{
    SDSPI.begin(MY_SCLK, MY_MISO, MY_MOSI, MY_CS);
    //Assuming use of SPI SD card
    if (!SD.begin(MY_CS, SDSPI)) {
        Serial.println("Card Mount Failed");
        tft.setTextColor(TFT_RED);
        tft.drawString("SDCard Mount FAIL", tft.width() / 2, tft.height() / 2 - 32);
        tft.setTextColor(TFT_GREEN);
    } else {
        tft.setTextColor(TFT_GREEN);
        Serial.println("SDCard Mount PASS");
        tft.drawString("SDCard Mount PASS", tft.width() / 2, tft.height() / 2 - 48);
        String size = String((uint32_t)(SD.cardSize() / 1024 / 1024)) + "MB";
        tft.drawString(size, tft.width() / 2, tft.height() / 2 - 32);
    }
}
#else
#define setupSDCard()
#endif

/*
void wifi_scan();
*/

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

/*
void showVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "Voltage :" + String(battery_voltage) + "V";
        Serial.println(voltage);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
    }
}
*/

float getEspBatVoltage(void)
{
    uint16_t v = analogRead(ADC_PIN);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    return battery_voltage;
}
void button_init()
{
    btnLeft.setLongClickHandler([](Button2 & b) {
        Serial.println("button left click");
        
        btnCick = false;
        int r = digitalRead(TFT_BL);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextFont(1);
        tft.setTextSize(2);
        tft.drawString("Press again",  tft.width() / 2, tft.height() / 2 - 10 );
        tft.drawString("to wake up",  tft.width() / 2, tft.height() / 2 + 10 );
        espDelay(3000);
        digitalWrite(TFT_BL, !r);

        tft.writecommand(TFT_DISPOFF);
        tft.writecommand(TFT_SLPIN);
        //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
        // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
        delay(200);
        esp_deep_sleep_start();
        
    });
    btnLeft.setPressedHandler([](Button2 & b) {
        Serial.println("button left");
        dbrd.prevPage();

        //btnCick = true;
    });

    btnRight.setPressedHandler([](Button2 & b) {
        Serial.println("button right");
        dbrd.nextPage();
        //btnCick = false;
        //wifi_scan();
        //bt_connect();
        
    });
}

void button_loop()
{
    btnLeft.loop();
    btnRight.loop();
}

/*
void wifi_scan()
{
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int16_t n = WiFi.scanNetworks();
    tft.fillScreen(TFT_BLACK);
    if (n == 0) {
        tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
    } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 0);
        Serial.printf("Found %d net\n", n);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            tft.println(buff);
        }
    }
    // WiFi.mode(WIFI_OFF);
}
*/


void setup()
{
    Serial.begin(115200);

    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);


    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    */

    tft.setSwapBytes(true);
    /*
    tft.pushImage(0, 0,  240, 135, ttgo);
    espDelay(5000);
*/

    tft.setRotation(0);
    tft.fillScreen(TFT_RED);
    espDelay(100);
    tft.fillScreen(TFT_BLUE);
    espDelay(100);
    tft.fillScreen(TFT_GREEN);
    espDelay(100);

    // init dashboard
    dbrd.init(&tft);

    button_init();

    
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    } else {
        Serial.println("Default Vref: 1100mV");
    }
    

    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.drawString("Please wait",  tft.width() / 2, tft.height() / 2 );

    // setupSDCard();

    /*
    tft.drawString("LeftButton:", tft.width() / 2, tft.height() / 2 - 16);
    tft.drawString("[WiFi Scan]", tft.width() / 2, tft.height() / 2 );
    tft.drawString("RightButton:", tft.width() / 2, tft.height() / 2 + 16);
    tft.drawString("[Voltage Monitor]", tft.width() / 2, tft.height() / 2 + 32 );
    tft.drawString("RightButtonLongPress:", tft.width() / 2, tft.height() / 2 + 48);
    tft.drawString("[Deep Sleep]", tft.width() / 2, tft.height() / 2 + 64 );
    tft.setTextDatum(TL_DATUM);
    */

   btConnected = bt_connect();
   if( btConnected ){
       dbrd.setBtConnectionStatus(BT_CONNECTED);
   };

}

void refreshVescDashboard(void)
{
  if (btConnected)
  {
    if ( VescConnection.getVescValues() ) 
    {
      //Serial.println(VescConnection.data.rpm);
      //Serial.println(VescConnection.data.inputVoltage);
      dbrd.setSpeed(VescConnection.data.rpm);
      dbrd.setBatCurrent(VescConnection.data.avgInputCurrent);
      dbrd.setBatVoltage(VescConnection.data.inputVoltage);
      dbrd.setDistance(VescConnection.data.tachometerAbsValue);
     // Serial.printf("Tacho: %d \n", VescConnection.data.tachometerAbsValue);
  
    }
  }
  int64_t currTime = esp_timer_get_time();
  if (currTime - lastDisplayUpdateTime > 50){
    dbrd.drawCurrentPage();
    lastDisplayUpdateTime = currTime;
  }
}
void loop()
{
    /*
    if (btnCick) {
        showVoltage();
    }
    */
    refreshVescDashboard();
    dbrd.setEspVoltage(getEspBatVoltage());
    button_loop();
}
