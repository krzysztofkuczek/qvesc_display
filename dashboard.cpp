
#include <TFT_eSPI.h>
//#include "Free_Fonts.h"
#include "fonts/TickingTimebombBB9pt7b.h"
#include "fonts/TickingTimebombBB12pt7b.h"
#include "fonts/TickingTimebombBB20pt7b.h"
#include "fonts/TickingTimebombBB24pt7b.h"
#include "fonts/TickingTimebombBB36pt7b.h"
#include "fonts/TickingTimebombBB40pt7b.h"
#include "dashboard.h"


void Dashboard::init(TFT_eSPI * tft){
    this->tft = tft;
};

void Dashboard::nextPage(void){
    if ( this->currentPage < PAGE_LAST - 1 )
    {
        this->currentPage++;
        refreshAll();
    }
};

void Dashboard::prevPage(void){
    if ( this->currentPage > PAGE_STATUS )
    {
        this->currentPage--;
        refreshAll();
    }
};

void Dashboard::refreshAll(void){
    needRefreshStatus = true;
    needRefreshPage01 = true;
    needRefreshPage02 = true;
    needRefreshPage03 = true;
}
void Dashboard::drawLabel(const char * text, const int32_t y){
   
    tft->setFreeFont(&TickingTimebombBB20pt7b);
    tft->setTextSize(1);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(DASHBOARD_LABEL_COLOR);
    tft->drawString(text,  tft->width() / 2, y );
};

void Dashboard::drawValue(const char * text, const char * oldText, const int32_t y){
   
    tft->setFreeFont(&TickingTimebombBB40pt7b);
    tft->setTextSize(1);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(DASHBOARD_BG_COLOR);
    tft->drawString(oldText,  tft->width() / 2, y );
    tft->setTextColor(DASHBOARD_VALUE_COLOR);
    tft->drawString(text,  tft->width() / 2, y );
};

void Dashboard::drawValueSmall(const char * text, const char * oldText, const int32_t y){
   
    tft->setFreeFont(&TickingTimebombBB24pt7b);
    tft->setTextSize(1);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(DASHBOARD_BG_COLOR);
    tft->drawString(oldText,  tft->width() / 2, y );
    tft->setTextColor(DASHBOARD_VALUE_COLOR);
    tft->drawString(text,  tft->width() / 2, y );
};

void Dashboard::drawUnit(const char * text, const int32_t y){
   
    tft->setFreeFont(&TickingTimebombBB12pt7b);
    tft->setTextSize(1);
    tft->setTextDatum(MR_DATUM);
    tft->setTextColor(DASHBOARD_UNIT_COLOR);
    tft->drawString(text,  tft->width()-10, y );
};

void Dashboard::drawStatusText(const char * text, const int32_t y){
   
    tft->setTextFont(1);
    tft->setTextSize(2);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(DASHBOARD_VALUE_COLOR);
    tft->drawString(text,  tft->width() / 2, y );
};


void Dashboard::drawCurrentPage(void){
    String curPageStr = "Current Page: " + String(this->currentPage);
    Serial.println(curPageStr);
       
    switch (this->currentPage)
    {
    case PAGE_STATUS:
        this->drawPageStatus();
        break;
    case PAGE_01:
        this->drawPage01();
        break;
    case PAGE_02:
        this->drawPage02();
        break;
    case PAGE_03:
        this->drawPage03();
        break;
    default:
        break;
    };
  
};

void Dashboard::drawPageStatus(void)
{
    if (this->needRefreshStatus){
        this->needRefreshStatus = false;
        tft->fillScreen(DASHBOARD_BG_COLOR);
        this->drawLabel("Status",12);
        tft->setTextFont(1);
        tft->setTextSize(2);
        switch (this->btConnectionStatus)
        {
            case BT_CONNECTED:
                drawStatusText("BT", 57);
                drawStatusText("connected", 77);
                break;
            case BT_NOT_CONNECTED:
                drawStatusText("BT not", 57);
                drawStatusText("connected", 77);
                break;
            default:
                break;
        }
    }
}

void Dashboard::drawPage01(void)
{
    char buff[32];
    char lastBuff[32];
    if (this->needRefreshPage01){
        tft->fillScreen(DASHBOARD_BG_COLOR);
        this->drawLabel("Speed",12);
        this->drawUnit("km/h",102);
        this->drawLabel("Current",130);
        this->drawUnit("Amp",225);
        }

    if (this->speed != this->lastSpeed || this->needRefreshPage01  ) {
        sprintf(buff,"%.1f", this->speed);
        sprintf(lastBuff,"%.1f", this->lastSpeed);
        this->drawValue(buff, lastBuff,57);
        this->lastSpeed = this->speed;
    };
    
    if (this->batCurrent != this->lastBatCurrent || this->needRefreshPage01 ){
        sprintf(buff,"%.1f", this->batCurrent);
        sprintf(lastBuff,"%.1f", this->lastBatCurrent);
        this->drawValue(buff, lastBuff, 180);
        this->lastBatCurrent = this->batCurrent;
    }

    if(this->needRefreshPage01){
        this->needRefreshPage01 = false;
    }
}

void Dashboard::drawPage02(void)
{
    char buff[32];
    char lastBuff[32];
    if (this->needRefreshPage02){
        tft->fillScreen(DASHBOARD_BG_COLOR);
        this->drawLabel("Battery",12);
        this->drawUnit("Volt",102);
        this->drawLabel("Esp Bat",130);
        this->drawUnit("Volt",225);
        }

    if (this->batVoltage!= this->lastBatVoltage || this->needRefreshPage02  ) {
        sprintf(buff,"%.1f", this->batVoltage);
        sprintf(lastBuff,"%.1f", this->lastBatVoltage);
        this->drawValue(buff, lastBuff,57);
        this->lastBatVoltage = this->batVoltage;
    };
    
    
    if (this->espVoltage != this->lastEspVoltage || this->needRefreshPage02 ){
        sprintf(buff,"%.1f", this->espVoltage);
        sprintf(lastBuff,"%.1f", this->lastEspVoltage);
        this->drawValue(buff, lastBuff, 180);
        this->lastEspVoltage = this->espVoltage;
    }
    

    if(this->needRefreshPage02){
        this->needRefreshPage02 = false;
    }

}

void Dashboard::drawPage03(void)
{
    char buff[32];
    char lastBuff[32];
    if (this->needRefreshPage03){
        tft->fillScreen(DASHBOARD_BG_COLOR);
        this->drawLabel("Distance",12);
        this->drawUnit("km",102);
//        this->drawLabel("Current",130);
//        this->drawUnit("Amp",225);
        }

    if (this->distance != this->lastDistance || this->needRefreshPage03  ) {
        sprintf(buff,"%.2f", this->distance);
        sprintf(lastBuff,"%.2f", this->lastDistance);
        this->drawValueSmall(buff, lastBuff,57);
        this->lastDistance = this->distance;
    };
/*    
    if (this->batCurrent != this->lastBatCurrent || this->needRefreshPage03 ){
        sprintf(buff,"%.1f", this->batCurrent);
        sprintf(lastBuff,"%.1f", this->lastBatCurrent);
        this->drawValue(buff, lastBuff, 180);
        this->lastBatCurrent = this->batCurrent;
    }
*/
    if(this->needRefreshPage03){
        this->needRefreshPage03 = false;
    }
}

void Dashboard::setSpeed(const float erpm){
    
    float currentSpeed = erpm2kmh(erpm);
    if (abs(this->speed - currentSpeed) >= 0.1)
    {
        this->speed = currentSpeed;
    }
       
}

void Dashboard::setDistance( const float tacho){
    float currentDistance = tacho2distance(tacho);
    if (abs(this->distance - currentDistance) >= 0.01)
    {
        this->distance = currentDistance;
    }
}
void Dashboard::setBatCurrent(const float current){
    if (abs(this->batCurrent - current) >= 0.1){
        this->batCurrent = current;
    }
}

void Dashboard::setBatVoltage(const float voltage){
    if (abs(this->batVoltage - voltage) >= 0.1){
        this->batVoltage = voltage;
    }
}

void Dashboard::setEspVoltage(const float voltage){
    if (abs(this->espVoltage - voltage) >= 0.1){
        this->espVoltage = voltage;
    }
}

float Dashboard::erpm2kmh(const float erpm){
    float rpm = erpm / MOTOR_POOLS;
    float wheelCircumference = 3.14159 * WHEEL_SIZE; // in mm
    float gearRatio = (float)WHEEL_PULLEY_TEETH / (float)MOTOR_PULLEY_TEETH;
    float wheelRpm = rpm/gearRatio;
    float speedMmMin = wheelRpm * wheelCircumference;
    float speedKmH = 60.0 * speedMmMin / 1000000;
    return speedKmH;

}

float Dashboard::tacho2distance(const float tacho){
    float noOfMotorRotations = tacho / MOTOR_POOLS;
    float wheelCircumference = 3.14159 * WHEEL_SIZE; // in mm
    float gearRatio = (float)WHEEL_PULLEY_TEETH / (float)MOTOR_PULLEY_TEETH;
    float distance = noOfMotorRotations * wheelCircumference / gearRatio / 1000000 / 3 / 2;
    return distance;

}


void Dashboard::setBtConnectionStatus(const int status){
    this->btConnectionStatus = status;
}