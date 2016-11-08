// -----------------------------------------------------------------------------
// OLED control: code for dealing with oled for the "Roost!" project
//               uses library by Daniel Eichhorn downloaded from github
//               https://github.com/squix78/esp8266-oled-ssd1306
//

// -------------------------------------
// initialize the oled
// -------------------------------------
void oled_setup(){
  display.init();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Liberation_Mono_10); // 21 characters per line

  display.flipScreenVertically(); // optional, puts the headers at the top of the screen
  display.drawString(0, 0, "Cock-a-doodle-doo!");
  //display.drawString(0, 10, "----5----0----5----0----5----0");
  display.display();
}

// -------------------------------------
// display roost data on the oled
// -------------------------------------
void oled_roost(){
  char hs[6] = "00.00";                 // humidity
  char ts[6] = "00.00";                 // temp Celsius
  char fs[6] = "00.00";                 // temp Fahrenheit
  char ls[22] = {};                     // line string

  Serial.println("refreshing OLED");

  display.clear();
  display.drawString(0, 0, wifi_ipaddr);

  dtostrf(h, 2, 2, &hs[0]);
  sprintf(ls,"Humidity %%%s", hs);
  display.drawString(0, 10, ls);

  dtostrf(t, 2, 2, &ts[0]);
  dtostrf(f, 2, 2, &fs[0]);
  sprintf(ls, "Temp c %s f %s", ts, fs);
  display.drawString(0, 20, ls);

  dtostrf(hic, 2, 2, &ts[0]);
  dtostrf(hif, 2, 2, &fs[0]);
  sprintf(ls, "Indx c %s f %s", ts, fs);
  display.drawString(0, 30, ls);

  sprintf(ls, "NTP Time %d", ntp_epoch_in_seconds);
  display.drawString(0, 40, ls);

  sprintf(ls, "Motion @ %d", pir_last_motion);
  display.drawString(0, 50, ls);
  display.display();
}

