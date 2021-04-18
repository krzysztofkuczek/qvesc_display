#ifndef __DASHBOARD_H__
#define __DASHBOARD_H__

#define DASHBOARD_BG_COLOR  TFT_LIGHTGREY
#define DASHBOARD_LABEL_COLOR  TFT_BLACK
#define DASHBOARD_VALUE_COLOR  TFT_RED
#define DASHBOARD_UNIT_COLOR  TFT_RED

#define MOTOR_PULLEY_TEETH  12
#define WHEEL_PULLEY_TEETH  32
#define WHEEL_SIZE          70          // in mm
#define MOTOR_POOLS         7           // no of magnets divided by 2

    enum Pages {
        PAGE_STATUS,
        PAGE_01,
        PAGE_02,
        PAGE_03,
        PAGE_LAST // not used
    };

    enum BTConnectionStatus {
        BT_NOT_CONNECTED,
        BT_CONNECTED
    };
  
    class Dashboard {
        
        private:
            int currentPage = PAGE_STATUS;
            TFT_eSPI * tft;
            int needRefreshStatus = 1;
            int needRefreshPage01 = 1;
            int needRefreshPage02 = 1;
            int needRefreshPage03 = 1;


            float speed = 0;
            float lastSpeed = 0;

            float batCurrent = 0;
            float lastBatCurrent = 0;

            float batVoltage = 0;
            float lastBatVoltage = 0;

            float espVoltage = 0;
            float lastEspVoltage = 0;

            float distance = 0;
            float lastDistance = 0;

            int btConnectionStatus = BT_NOT_CONNECTED;
            

            void drawLabel(const char * text, const int32_t y);
            void drawValue(const char * text, const char * oldText, const int32_t y);
            void drawValueSmall(const char * text, const char * oldText, const int32_t y);
            void drawUnit(const char * text, const int32_t y);
            void drawStatusText(const char * text, const int32_t y);
            float erpm2kmh(const float erpm);
            float tacho2distance(const float tacho);
            void drawPageStatus(void);
            void drawPage01(void);
            void drawPage02(void);
            void drawPage03(void);
            void refreshAll(void);
        public:
            void init(TFT_eSPI * tft);
            void nextPage(void);
            void prevPage(void);
            void drawCurrentPage(void);
            void setSpeed(const float erpm);
            void setBatCurrent(const float current);
            void setBatVoltage(const float voltage);
            void setEspVoltage(const float voltage);
            void setDistance(const float tacho);
            void setBtConnectionStatus(const int status);
            
            
    };

#endif
