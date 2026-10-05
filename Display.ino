void updateOLED() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.printf("%d%%", (int)batPercent);

  display.setTextSize(1);
  if (hasDirectPower) {
    if (chargeComplete) {
      display.setCursor(55, 0); display.print("[ FULL ]");
    } else if (isCharging) {
      display.setCursor(55, 0); display.print("[CHARGING]");
      display.setCursor(55, 9); display.printf("In:%.1fA", i_bat);
    } else {
      display.setCursor(55, 4); display.print("[MODE: UPS]");
    }
  } else {
    display.setCursor(55, 4); display.printf("LOAD:%.1fW", p_bat);
  }

  display.drawFastHLine(0, 18, 128, SSD1306_WHITE);
  display.drawFastVLine(64, 18, 34, SSD1306_WHITE);
  display.drawFastHLine(0, 52, 128, SSD1306_WHITE);

  display.setCursor(0, 24);  display.printf("L1: %s", relay1State ? "ON " : "OFF");
  display.setCursor(0, 38);  display.printf("L2: %s", relay2State ? "ON " : "OFF");

  display.setCursor(70, 24); display.print("V-IN:");
  display.setCursor(70, 38); display.printf("%.1f V", v_dc_in);

  display.setCursor(0, 55);  display.printf("V:%.2f", v_bat);
  if(tempBank1 > -50.0f && tempBank1 < 100.0f) { 
      display.setCursor(75, 55); display.printf("T:%.1fC", tempBank1);
  }

  display.display();
  oledNeedsUpdate = false;
}