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
      isChargePaused = false;
      lastChargeCycleStart = now;
    }

    if (chargeComplete) {
      enableIP2365Charge(false);
      currentSetPoint = 0.0f;
      return;
    }

    if (!isChargePaused && (now - lastChargeCycleStart > 60000)) { 
      enableIP2365Charge(false); 
      isChargePaused = true;
      lastChargePauseTime = now;
    } 
    else if (isChargePaused && (now - lastChargePauseTime > 2000)) { 
      if (v_bat >= BAT_MAX_V) { 
        chargeComplete = true; 
        enableIP2365Charge(false);
      } else {
        isChargePaused = false;
        lastChargeCycleStart = now;
        enableIP2365Charge(true); 
      }
    }

    if (!isChargePaused && !chargeComplete) {
      float target = chargeManualCurrent;
      
      if (chargeAutoMode) {
        float maxTemp = max(tempBank1, tempBank2);
        if (maxTemp > 45.0f || tempCharger > 60.0f) {
          target = 0.0f; 
        } else if (maxTemp > 35.0f || maxTemp < 5.0f) {
          target = 0.5f; 
        } else {
          target = 1.15f; 
        }
      }
      
      currentSetPoint = target;

      if (target < 0.1f) {
        enableIP2365Charge(false);
      } else {
        uint8_t hexVal = round((target - 0.05f) / 0.1f);
        writeIP2365(0x1F, hexVal);
        enableIP2365Charge(true);
      }
    }
  } else {
    ip2365_online = false;
    ip2365_initialized = false;
  }
}