// =============================================================================
// File: Roost.ino
// Desc: Roost! An open source implementaion of a temperature and motion
//       monitoring station based on an ESP8266 with DHT11 and HC-SR501 sensors.
//
//       This code is in the public domain
// =============================================================================
#include "src/led.h"
#include "src/serial.h"
#include "src/dht.h"
#include "src/oled.h"
#include "src/pir.h"
#include "src/wifi.h"
#include "src/ntp.h"
#include "src/web.h"

void setup() {

  // leds off, please
  led_setup();

  // Serial for debugging
  // serial_setup();

  // digital humididy temperature
  // dht_setup();
  
  // OLED
  // oled_setup();
  
  // PIR
  // pir_setup();

  // WiFi
  // wifi_setup();

  // NTP time
  // ntp_setup();

  // web
  // web_setup();
}
// =============================================================================
void loop() {
  static int display_last = 0;
  static int ntp_last = 0;

  // clear the roost
  // dht_clear();

  // get humidity and temp
  // dht_read();

  // check for motion
  // pir_chk_motion();

  // keep time updated
  // ntp_epoch_in_seconds += (millis() / 1000);

  // update display and serial every 5 seconds
  // if (millis() - display_last > 5000) {
  //     display_last = millis();
  //     oled_roost();
  //     serial_roost();
  // }

  // update time every 60 seconds or when the last request is incomplete
  // if ((millis() - ntp_last) > 60000 || (! ntp_packet_received)){
  //   ntp_last = millis();
  //   ntp_send_request();
  // }

  // let the web server do its thing every iteration
  // web_server.handleClient();
}
// =============================================================================

