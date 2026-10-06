void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void handleRelay() {
  if (server.hasArg("ch")) {
    int ch = server.arg("ch").toInt();
    if (ch == 1) { relay1State = !relay1State; setRelay(RELAY1_PIN, relay1State); }
    if (ch == 2) { relay2State = !relay2State; setRelay(RELAY2_PIN, relay2State); }
    oledNeedsUpdate = true;
  }
  server.send(200, "text/plain", "OK");
}

void handleChargerConfig() {
  if (server.hasArg("auto")) {
    chargeAutoMode = server.arg("auto").toInt() == 1;
  }
  if (server.hasArg("en")) {
    chargerMasterSwitch = server.arg("en").toInt() == 1;
  }
  if (server.hasArg("current")) {
    chargeManualCurrent = server.arg("current").toFloat();
    // محدودکننده به سقف ۲ آمپر تغییر یافت
    if (chargeManualCurrent > 2.0f) chargeManualCurrent = 2.0f;
    if (chargeManualCurrent < 0.1f) chargeManualCurrent = 0.1f;
  }
  server.send(200, "text/plain", "OK");
}

void handleData() {
  String json = "{\"v1\":"      + String(v_bat, 2) +
                ",\"i1\":"      + String(i_bat, 2) +
                ",\"p1\":"      + String(p_bat, 2) +
                ",\"v2\":"      + String(v_dc_in, 2) +
                ",\"i2\":"      + String(i_dc_in, 2) +
                ",\"p2\":"      + String(p_dc_in, 2) +
                ",\"v3\":"      + String(v_mob, 2) +
                ",\"i3\":"      + String(i_mob, 2) +
                ",\"p3\":"      + String(p_mob, 2) +
                ",\"mah\":"     + String(mAhUsed, 0) +
                ",\"pct\":"     + String(batPercent, 1) +
                ",\"t_min\":"   + String(timeMinutes, 1) +
                ",\"tb1\":"     + String(tempBank1, 1) +
                ",\"tb2\":"     + String(tempBank2, 1) +
                ",\"tchg\":"    + String(tempCharger, 1) +
                ",\"charging\":"+ String(isCharging ? 1 : 0) +
                ",\"dc_in\":"   + String(hasDirectPower ? 1 : 0) +
                ",\"r1\":"      + String(relay1State ? 1 : 0) +
                ",\"r2\":"      + String(relay2State ? 1 : 0) + 
                ",\"ip_on\":"   + String(ip2365_online ? 1 : 0) +
                ",\"ip_mod\":"  + String(chargeAutoMode ? 1 : 0) +
                ",\"ip_en\":"   + String(chargerMasterSwitch ? 1 : 0) + 
                ",\"ip_cur\":"  + String(chargeManualCurrent, 2) +
                ",\"ip_tgt\":"  + String(currentSetPoint, 2) +
                ",\"ip_st\":"   + String(chargeComplete ? 2 : 1) + "}";
  server.send(200, "application/json", json);
}