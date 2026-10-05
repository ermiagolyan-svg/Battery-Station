#include "Config.h"
#include "WebPanel.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <INA3221.h>
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(80);
INA3221 ina3221(INA3221_ADDR, &Wire);

// ─── متغیرهای گلوبال ──────────────────────────────────
bool relay1State = false;
bool relay2State = false;

float v_bat = 0.0f, i_bat = 0.0f, p_bat = 0.0f; 
float v_dc_in = 0.0f, i_dc_in = 0.0f, p_dc_in = 0.0f; 
float v_mob = 0.0f, i_mob = 0.0f, p_mob = 0.0f; 

float tempBank1 = 0.0f, tempBank2 = 0.0f, tempCharger = 0.0f;

float mAhUsed         = 0.0f;
float batPercent      = 0.0f;
float timeMinutes     = -1.0f; 
bool  isCharging      = false;
bool  hasDirectPower  = false; 
bool  oledNeedsUpdate = true;

bool rf1Last = LOW, rf2Last = LOW;
bool rf3Last = LOW, rf4Last = LOW;

unsigned long lastMeasure = 0;
unsigned long lastOLED    = 0;
unsigned long bootTime    = 0;

bool  ip2365_online        = false;
bool  ip2365_initialized   = false;
bool  chargeAutoMode       = true;
bool  chargerMasterSwitch  = true;
float chargeManualCurrent  = 1.15f;
float currentSetPoint      = 0.0f;
bool  isChargePaused       = false;
bool  chargeComplete       = false;
unsigned long lastChargePauseTime  = 0;
unsigned long lastChargeCycleStart = 0;

void setRelay(int pin, bool state) {
  digitalWrite(pin, state ? LOW : HIGH);
}

void checkRF() {
  if (millis() - bootTime < RF_IGNORE_AFTER_BOOT) return;

  if (digitalRead(RF_VT) == HIGH) {
    bool ch1 = digitalRead(RF_CH1); // D3 (معمولاً کلید D)
    bool ch2 = digitalRead(RF_CH2); // D2 (معمولاً کلید C)
    bool ch3 = digitalRead(RF_CH3); // D1 (معمولاً کلید B)
    bool ch4 = digitalRead(RF_CH4); // D0 (معمولاً کلید A)

    // در حالت پول‌داون، کلید فشرده شده HIGH می‌شود
    if (ch4 == HIGH && rf4Last == LOW) { relay1State = !relay1State; setRelay(RELAY1_PIN, relay1State); oledNeedsUpdate = true; } // کلید A: مدار اول
    if (ch3 == HIGH && rf3Last == LOW) { relay2State = !relay2State; setRelay(RELAY2_PIN, relay2State); oledNeedsUpdate = true; } // کلید B: مدار دوم
    if (ch2 == HIGH && rf2Last == LOW) { relay1State = false; relay2State = false; setRelay(RELAY1_PIN, false); setRelay(RELAY2_PIN, false); oledNeedsUpdate = true; } // کلید C: همه خاموش
    if (ch1 == HIGH && rf1Last == LOW) { relay1State = true;  relay2State = true;  setRelay(RELAY1_PIN, true);  setRelay(RELAY2_PIN, true);  oledNeedsUpdate = true; } // کلید D: همه روشن

    rf1Last = ch1; rf2Last = ch2; rf3Last = ch3; rf4Last = ch4;
  } else {
    rf1Last = LOW; 
    rf2Last = LOW; 
    rf3Last = LOW; 
    rf4Last = LOW;
  }
}

void setup() {
  Serial.begin(115200);
  bootTime = millis();

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  setRelay(RELAY1_PIN, false);
  setRelay(RELAY2_PIN, false);

  pinMode(RF_VT,  INPUT); 
  pinMode(RF_CH1, INPUT_PULLDOWN);
  pinMode(RF_CH2, INPUT_PULLDOWN);
  pinMode(RF_CH3, INPUT_PULLDOWN);
  pinMode(RF_CH4, INPUT_PULLDOWN);

  sensors.begin();

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  
  ina3221.begin();
  ina3221.setShuntR(0, SHUNT_CH1);
  ina3221.setShuntR(1, SHUNT_CH2);
  ina3221.setShuntR(2, SHUNT_CH3);

  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    display.setRotation(2);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(15, 25);
    display.println("BMS SMART STATION");
    display.display();
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, 1, 0, 4);

  server.on("/",              handleRoot);
  server.on("/relay",         handleRelay);
  server.on("/data",          handleData);
  server.on("/chargerConfig", handleChargerConfig);
  server.begin();

  lastMeasure = millis();
}

void loop() {
  server.handleClient();
  checkRF();

  unsigned long now = millis();

  if (now - lastMeasure >= 1000) {
    readSensors();
  }
  
  manageIP2365();

  if (oledNeedsUpdate || (now - lastOLED >= 1500)) {
    updateOLED();
    lastOLED = now;
  }
}