// ─── توابع ارتباط با IP2365 ───────────────────
uint8_t readIP2365(uint8_t reg) {
  Wire.beginTransmission(IP2365_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  Wire.requestFrom((uint16_t)IP2365_ADDR, (uint8_t)1, true);
  if (Wire.available()) return Wire.read();
  return 0;
}

void writeIP2365(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(IP2365_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void enableIP2365Charge(bool enable) {
  uint8_t r01 = readIP2365(0x01);
  if (enable) r01 |= 0x01;  
  else        r01 &= ~0x01; 
  writeIP2365(0x01, r01);
}

void manageIP2365() {
  static unsigned long lastIPCheck = 0;
  static unsigned long lastCVAdjust = 0;
  unsigned long now = millis();
  
  if (now - lastIPCheck < 1000) return; 
  lastIPCheck = now;

  Wire.beginTransmission(IP2365_ADDR);
  if (Wire.endTransmission() == 0) {
    ip2365_online = true;

    if (!ip2365_initialized) {
      uint8_t r0F = readIP2365(0x0F);
      writeIP2365(0x0F, r0F & ~0x0C); 
      
      uint8_t r05 = readIP2365(0x05);
      writeIP2365(0x05, r05 & ~0x01); 

      ip2365_initialized = true;
      chargeComplete = false;
    }

    if (chargeComplete) {
      // اگر باتری 0.4 ولت افت کرد، دوباره سیکل شارژ از سر گرفته بشه
      if (v_bat < BAT_MAX_V - 0.4f) {
        chargeComplete = false;
      } else {
        enableIP2365Charge(false);
        currentSetPoint = 0.0f;
        return;
      }
    }

    // محاسبه جریان هدف اولیه
    float target = chargeManualCurrent; 

    if (chargeAutoMode) {
      // کنترل دمای باتری (اولویت اول)
      float maxBatTemp = max(tempBank1, tempBank2);
      if (maxBatTemp > 45.0f) {
        target = 0.0f; 
      } else if (maxBatTemp > 35.0f || maxBatTemp < 5.0f) {
        target = 0.5f; 
      } else {
        target = 3.0f; // شارژ با حداکثر سرعت 3 آمپر در حالت اتوماتیک
      }

      // کنترل حرارتی نزولی هیت‌سینک شارژر
      if (tempCharger >= 90.0f) {
        target = 0.0f; // قطع کامل در 90 درجه
      } else if (tempCharger > 75.0f) {
        // کاهش خطی جریان بین 75 تا 90 درجه (در 85 درجه حدود 33٪ جریان تارگت اعمال میشه)
        float derateFactor = (90.0f - tempCharger) / 15.0f;
        target = target * derateFactor;
      }
    }

    // الگوریتم هوشمند CC/CV (بدون قطع و وصل رله)
    if (target > 0.0f) {
      if (now - lastCVAdjust > 2000) { // تنظیم نرم هر 2 ثانیه برای جلوگیری از نوسان
        if (v_bat >= BAT_MAX_V) {
          currentSetPoint -= 0.1f; // فاز CV: کاهش پله‌ای جریان برای تثبیت ولتاژ
        } else if (v_bat < BAT_MAX_V - 0.05f) {
          currentSetPoint += 0.1f; // فاز CC: افزایش جریان تا رسیدن به تارگت مجاز
        }
        lastCVAdjust = now;
      }
      
      // کلمپ کردن (محدود کردن) جریان بین صفر و تارگت محاسبه‌شده
      if (currentSetPoint > target) currentSetPoint = target;
      if (currentSetPoint < 0.1f) currentSetPoint = 0.0f;

      // تشخیص پایان شارژ (ولتاژ در سقف و جریان بسیار ضعیف شده)
      if (v_bat >= BAT_MAX_V - 0.05f && currentSetPoint <= 0.15f && currentSetPoint > 0.0f) {
         chargeComplete = true;
         enableIP2365Charge(false);
         currentSetPoint = 0.0f;
         return;
      }
      
    } else {
      currentSetPoint = 0.0f;
    }

    // اعمال نهایی جریان به آی‌سی
    if (currentSetPoint < 0.1f) {
      enableIP2365Charge(false);
    } else {
      uint8_t hexVal = round((currentSetPoint - 0.05f) / 0.1f);
      writeIP2365(0x1F, hexVal);
      enableIP2365Charge(true);
    }
    
  } else {
    ip2365_online = false;
    ip2365_initialized = false;
  }
}