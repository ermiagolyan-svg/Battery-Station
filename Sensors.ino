void readSensors() {
  unsigned long now = millis();
  float dt_h = (now - lastMeasure) / 3600000.0f;
  lastMeasure = now;

  sensors.requestTemperatures();
  tempBank1 = sensors.getTempCByIndex(2);
  tempBank2 = sensors.getTempCByIndex(1);
  tempCharger = sensors.getTempCByIndex(0);

  // خواندن مقادیر خام
  float raw_v_bat = ina3221.getBusVoltage(0);
  float raw_i_bat = ina3221.getCurrent(0); 
  
  float raw_v_dc = ina3221.getBusVoltage(1);
  float raw_i_dc = ina3221.getCurrent(1);

  float raw_v_mob = ina3221.getBusVoltage(2);
  float raw_i_mob = ina3221.getCurrent(2);

  // حذف نشتی کانال‌های ورودی/خروجی
  if (raw_v_dc < 1.0f) raw_v_dc = 0.0f;
  if (raw_i_dc < 0.02f) raw_i_dc = 0.0f;
  if (raw_v_mob < 1.0f) raw_v_mob = 0.0f;
  if (raw_i_mob < 0.02f) raw_i_mob = 0.0f;

  if (v_bat == 0.0f) v_bat = raw_v_bat;
  else v_bat = (0.1f * raw_v_bat) + (0.9f * v_bat);

  // فیلتر فوق‌سنگین برای خنثی کردن نوسانات مصرف خود ESP32 و وای‌فای
  if (raw_i_bat < -0.04f) {
      isCharging = true;
      float target_i = abs(raw_i_bat);
      if (i_bat == 0.0f) i_bat = target_i;
      else i_bat = (0.05f * target_i) + (0.95f * i_bat); // ضریب کند 0.05
  } else {
      isCharging = false;
      // نادیده گرفتن مصرف زیر 80 میلی‌آمپر (جریان بی‌باری میکروکنترلر)
      float target_i = (raw_i_bat > 0.08f) ? raw_i_bat : 0.0f; 
      if (i_bat == 0.0f) i_bat = target_i;
      else i_bat = (0.05f * target_i) + (0.95f * i_bat);
  }

  v_dc_in = raw_v_dc;
  i_dc_in = raw_i_dc;
  p_dc_in = v_dc_in * i_dc_in;

  v_mob = raw_v_mob;
  i_mob = raw_i_mob;
  p_mob = v_mob * i_mob;

  p_bat = v_bat * i_bat;
  hasDirectPower = (v_dc_in > 11.0f);

  batPercent = ((v_bat - BAT_MIN_V) / (BAT_MAX_V - BAT_MIN_V)) * 100.0f;
  batPercent = constrain(batPercent, 0.0f, 100.0f);

  // کالیبراسیون 4S
  float packCapacity4S = BAT_CAPACITY_MAH / 4.0f;
  float target_time = -1.0f;

  // محاسبه منطق زمان بر اساس ظرفیت کالیبره‌شده
  if (hasDirectPower) {
    if (isCharging) {
      float missingCapacity = packCapacity4S - (packCapacity4S * (batPercent / 100.0f));
      if (i_bat > 0.05f) {
        target_time = (missingCapacity / (i_bat * 1000.0f)) * 60.0f;
      }
    } else {
      target_time = -2.0f; // UPS
    }
  } else {
    if (dt_h > 0.0f && !isCharging) {
      mAhUsed += i_bat * 1000.0f * dt_h; 
    }
    float currentCapacity = packCapacity4S * (batPercent / 100.0f);
    if (i_bat > 0.05f) {
      target_time = (currentCapacity / (i_bat * 1000.0f)) * 60.0f;
    }
  }

  // فیلتر کردن خروجی زمان برای جلوگیری از پرش‌های اعصاب‌خردکن در داشبورد
  if (target_time < 0.0f) {
     timeMinutes = target_time; // اعمال فوری استاتوس‌های متنی
  } else {
     // اگر بار اول است، مساوی قرار بده، در غیر این صورت با ضریب 0.02 به نرمی حرکت بده
     if (timeMinutes < 0.0f || timeMinutes > 10000.0f) timeMinutes = target_time; 
     else timeMinutes = (0.02f * target_time) + (0.98f * timeMinutes); 
  }
}