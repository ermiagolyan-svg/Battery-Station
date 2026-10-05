#ifndef CONFIG_H
#define CONFIG_H

// ─── پین‌های سیستم ───────────────
#define RELAY1_PIN    25 
#define RELAY2_PIN    33 

#define RF_VT         36 
#define RF_CH1        23 
#define RF_CH2        22
#define RF_CH3        21 
#define RF_CH4        19 

#define SDA_PIN       16 
#define SCL_PIN       4  

#define ONE_WIRE_BUS  32 

// ─── تنظیمات ──────────────────────────────────
#define SCREEN_WIDTH         128
#define SCREEN_HEIGHT        64
#define OLED_ADDR            0x3C
#define INA3221_ADDR         0x40 
#define IP2365_ADDR          0x75 

#define BAT_MIN_V            12.0f
#define BAT_MAX_V            16.4f 
#define BAT_CAPACITY_MAH     20000.0f 
#define RF_IGNORE_AFTER_BOOT 3000

#define SHUNT_CH1  0.007f 
#define SHUNT_CH2  0.007f 
#define SHUNT_CH3  0.020f 

const char* const WIFI_SSID     = "Battery_Station_BMS";
const char* const WIFI_PASSWORD = "29552955";

#endif