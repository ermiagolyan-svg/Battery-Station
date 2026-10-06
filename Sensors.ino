void readSensors() {
  unsigned long now = millis();
  float dt_h = (now - lastMeasure) / 3600000.0f;
  lastMeasure = now;

  sensors.requestTemperatures();
  tempBank1 = sensors.getTempCByIndex(2);
  tempBank2 = sensors.getTempCByIndex(1);
  tempCharger = sensors.getTempCByIndex(0);

  // آرایه‌هایی برای ذخیره داده‌های فیلتر نویز
  float sum_i[3] = {0, 0, 0};
  float min_i[3] = {999.0f, 999.0f, 999.0f};
  float max_i[3] = {-999.0f, -999.0f, -999.0f};

  // خواندن ۵ نمونه بسیار سریع (Trimmed Mean) برای حذف نویز مگنت و سوییچینگ
  for(int i = 0; i < 5; i++) {
      for(int ch = 0; ch < 3; ch++) {
          float val = ina3221.getCurrent(ch);
          sum_i[ch] += val;
          if(val < min_i[ch]) min_i[ch] = val;
          if(val > max_i[ch]) max_i[ch] = val;
      }
      delay(2); // مکث ۲ میلی‌ثانیه‌ای برای عبور سریع از اسپایک نویز
  }

  // حذف بالاترین و پایین‌ترین مقدار (نویز مطلق) و میانگین‌گیری ۳ مقدار پایدار
  float raw_i_bat = (sum_i[0] - min_i[0] - max_i[0]) / 3.0f;
  float raw_i_dc  = (sum_i[1] - min_i[1] - max_i[1]) / 3.0f;
  float raw_i_mob = (sum_i[2] - min_i[2] - max_i[2]) / 3.0f;

  // ولتاژها نویز کمتری می‌گیرند، خواندن عادی کافیست
  float raw_v_bat = ina3221.getBusVoltage(0);
  float raw_v_dc  = ina3221.getBusVoltage(1);
  float raw_v_mob = ina3221.getBusVoltage(2);

  // حذف نشتی کانال‌های ورودی/خروجی
  if (raw_v_dc < 1.0f) raw_v_dc = 0.0f;
  if (raw_i_dc < 0.02f) raw_i_dc = 0.0f;
  if (raw_v_mob < 1.0f) raw_v_mob = 0.0f;
  if (raw_i_mob < 0.02f) raw_i_mob = 0.0f;

  // انتقال مستقیم دیتای خالص
  v_bat = raw_v_bat;
  v_dc_in = raw_v_dc;
  i_dc_in = raw_i_dc;
  v_mob = raw_v_mob;
  i_mob = raw_i_mob;

  // تشخیص وضعیت شارژ و اعمال مستقیم جریان باتری
  if (raw_i_bat < -0.04f) {
      isCharging = true;
      i_bat = abs(raw_i_bat);
  } else {
      isCharging = false;
      // نادیده گرفتن مصرف زیر 80 میلی‌آمپر (جریان بی‌باری میکروکنترلر)
      i_bat = (raw_i_bat > 0.08f) ? raw_i_bat : 0.0f; 
  }

  // محاسبه مستقیم توان 
  p_dc_in = v_dc_in * i_dc_in;
  p_mob = v_mob * i_mob;
  p_bat = v_bat * i_bat;
  hasDirectPower = (v_dc_in > 11.0f);

  batPercent = ((v_bat - BAT_MIN_V) / (BAT_MAX_V - BAT_MIN_V)) * 100.0f;
  batPercent = constrain(batPercent, 0.0f, 100.0f);

  // کالیبراسیون 4S
  float packCapacity4S = BAT_CAPACITY_MAH / 4.0f;
  float target_time = -1.0f;

  if (hasDirectPower) {
    if (isCharging) {
      float missingCapacity = packCapacity4S - (packCapacity4S * (batPercent / 100.0f));
      if (i_bat > 0.20f) {
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
    if (i_bat > 0.20f) {
      target_time = (currentCapacity / (i_bat * 1000.0f)) * 60.0f;
    }
  }

  // انتقال مستقیم زمان بدون تأخیر
  timeMinutes = target_time; 
}