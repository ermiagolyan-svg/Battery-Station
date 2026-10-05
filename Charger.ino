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
      writeIP2365(0x0F, r0F & ~0x0C); // باز کردن قفل سخت‌افزاری مقاومت‌ها
      
      uint8_t r05 = readIP2365(0x05);
      writeIP2365(0x05, r05 & ~0x01); 

      ip2365_initialized = true;
      chargeComplete = false;
    }

    bool shouldCharge = true;

    if (!chargerMasterSwitch) {
      shouldCharge = false;
    }

    if (chargeComplete) {
      if (v_bat < BAT_MAX_V - 0.4f) {
        chargeComplete = false;
      } else {
        shouldCharge = false;
      }
    }

    float target = 0.0f;

    if (shouldCharge) {
      target = chargeManualCurrent; 

      if (chargeAutoMode) {
        float maxBatTemp = max(tempBank1, tempBank2);
        if (maxBatTemp > 45.0f) {
          target = 0.0f; 
        } else if (maxBatTemp > 35.0f || maxBatTemp < 5.0f) {
          target = 0.5f; 
        } else {
          target = 3.0f; 
        }

        if (tempCharger >= 90.0f) {
          target = 0.0f; 
        } else if (tempCharger > 75.0f) {
          float derateFactor = (90.0f - tempCharger) / 15.0f;
          target = target * derateFactor;
        }
      }

      if (target > 0.0f) {
        if (now - lastCVAdjust > 2000) { 
          if (v_bat >= BAT_MAX_V) {
            currentSetPoint -= 0.1f; 
          } else if (v_bat < BAT_MAX_V - 0.05f) {
            currentSetPoint += 0.1f; 
          }
          lastCVAdjust = now;
        }
        
        if (currentSetPoint > target) currentSetPoint = target;
        if (currentSetPoint < 0.1f) currentSetPoint = 0.0f;

        if (v_bat >= BAT_MAX_V - 0.05f && currentSetPoint <= 0.15f && currentSetPoint > 0.0f) {
           chargeComplete = true;
           shouldCharge = false;
        }
        
      } else {
        currentSetPoint = 0.0f;
        shouldCharge = false;
      }
    }

    // ارسال دائم فرامین به آی‌سی (جلوگیری از تایم‌اوت و اعمال قطعی خاموشی)
    if (!shouldCharge || currentSetPoint < 0.1f) {
      uint8_t current_r1F = readIP2365(0x1F); 
      uint8_t new_r1F = (current_r1F & 0xC0) | 0x00; // تزریق صفر مطلق به رجیستر جریان
      writeIP2365(0x1F, new_r1F); 
      enableIP2365Charge(false);
      currentSetPoint = 0.0f;
    } else {
      uint8_t hexVal = round((currentSetPoint - 0.05f) / 0.1f);
      if (hexVal > 0x3F) hexVal = 0x3F;
      
      uint8_t current_r1F = readIP2365(0x1F); 
      uint8_t new_r1F = (current_r1F & 0xC0) | hexVal; 
      writeIP2365(0x1F, new_r1F); 
      enableIP2365Charge(true);
    }
    
  } else {
    ip2365_online = false;
    ip2365_initialized = false;
  }
}