void readSensors() {
  unsigned long now = millis();
  float dt_h = (now - lastMeasure) / 3600000.0f;
  lastMeasure = now;

  sensors.requestTemperatures();
  tempBank1 = sensors.getTempCByIndex(0);
  tempBank2 = sensors.getTempCByIndex(1);
  tempCharger = sensors.getTempCByIndex(2);

  v_bat = ina3221.getBusVoltage(0);
  float raw_i_bat = ina3221.getCurrent(0); 
  
  v_dc_in = ina3221.getBusVoltage(1);
  i_dc_in = ina3221.getCurrent(1);
  if (i_dc_in < 0.0f) i_dc_in = 0.0f;
  p_dc_in = v_dc_in * i_dc_in;

  v_mob = ina3221.getBusVoltage(2);
  i_mob = ina3221.getCurrent(2);
  if (i_mob < 0.0f) i_mob = 0.0f;
  p_mob = v_mob * i_mob;

  hasDirectPower = (v_dc_in > 11.0f);
  
  if (raw_i_bat < -0.04f) {
      isCharging = true;
      i_bat = abs(raw_i_bat); 
      p_bat = v_bat * i_bat;
  } else {
      isCharging = false;
      i_bat = (raw_i_bat > 0.0f) ? raw_i_bat : 0.0f;
      p_bat = v_bat * i_bat;
  }

  batPercent = ((v_bat - BAT_MIN_V) / (BAT_MAX_V - BAT_MIN_V)) * 100.0f;
  batPercent = constrain(batPercent, 0.0f, 100.0f);

  if (hasDirectPower) {
    if (isCharging) {
      float missingCapacity = BAT_CAPACITY_MAH - (BAT_CAPACITY_MAH * (batPercent / 100.0f));
      if (i_bat > 0.05f) {
        timeMinutes = (missingCapacity / (i_bat * 1000.0f)) * 60.0f;
      } else {
        timeMinutes = -1.0f;
      }
    } else {
      timeMinutes = -2.0f; 
    }
  } else {
    if (dt_h > 0.0f && !isCharging) {
      mAhUsed += i_bat * 1000.0f * dt_h;
    }
    float currentCapacity = BAT_CAPACITY_MAH * (batPercent / 100.0f);
    if (i_bat > 0.05f) {
      timeMinutes = (currentCapacity / (i_bat * 1000.0f)) * 60.0f;
    } else {
      timeMinutes = -1.0f; 
    }
  }
}