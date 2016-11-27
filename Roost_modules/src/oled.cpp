// -----------------------------------------------------------------------------
// OLED control: code for dealing with oled for the "Roost!" project
//               uses library by Daniel Eichhorn downloaded from github
//               https://github.com/squix78/esp8266-oled-ssd1306
//
#include "SSD1306.h"
#include "SSD1306Brzo.h"
#include "Liberation_Mono.h"
// #include "images.h"

// Initialize the OLED display using brzo_i2c
// D2 -> SDA
// D14 -> SCL
SSD1306Brzo display(0x3c, 2, 14);

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

  // ip address on line 1
  display.drawString(0, 0, wifi_ipaddr);

  // time on line 2
  sprintf(ls, "GMT %s", ntp_hms);
  display.drawString(0, 10, ls);

  // temperature on line 2
  dtostrf(t, 2, 2, &ts[0]);
  dtostrf(f, 2, 2, &fs[0]);
  sprintf(ls, "Temp c %s f %s", ts, fs);
  display.drawString(0, 20, ls);

  // humidity on line 4
  dtostrf(h, 2, 2, &hs[0]);
  sprintf(ls,"Humidity %%%s", hs);
  display.drawString(0, 30, ls);

  // heat index on line 5
  dtostrf(hic, 2, 2, &ts[0]);
  dtostrf(hif, 2, 2, &fs[0]);
  sprintf(ls, "Indx c %s f %s", ts, fs);
  display.drawString(0, 40, ls);

  // last motion on line 6
  sprintf(ls, "Motion @ %s", ntp_epoch2hms(pir_last_motion));
  display.drawString(0, 50, ls);

  display.display();
}
