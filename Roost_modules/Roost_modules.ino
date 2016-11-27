// =============================================================================
// File: Roost.ino
// Desc: Roost! An open source implementaion of a temperature and motion
//       monitoring station based on an ESP8266 with DHT22 and HC-SR501 sensors.
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
#include "src/iot.h"

// =============================================================================
void setup() {

  // leds off, please
  led_setup();

  // serial for debugging
  serial_setup();

  // WiFi
  wifi_setup();

  // digital humididy temperature
  dht_setup();

  // PIR
  pir_setup();

  // SR04
  sr_setup();

  // OLED
  oled_setup();

  // NTP
  ntp_setup();

  // WWW
  web_setup();

  // IoT
  iot_setup();
}

// =============================================================================
void loop() {
  static int display_last = 0;
  static int iot_last = 0;
  static int sr_last = 0;

  // get humidity and temp
  dht_clear();
  dht_read();

  // check for motion
  pir_chk_motion();

  // check proximity every 1 second using ultrasonic sensor
  if (millis() - sr_last > 1000) {
    sr_last = millis();
    sr_ping();
  }

  // request NTP time every 60 seconds
  if ((millis() - ntp_received_millis) > 60000){
    ntp_send_request();
  }

  // check for ntp response if not received
  if (! ntp_packet_received){ ntp_read_response(); };

  // keep time updated, last epoch received plus usec since last epoch received / 1000
  ntp_epoch_in_seconds = ntp_received_epoch + floor((millis() - ntp_received_millis) / 1000);
  ntp_hms = ntp_epoch2hms(ntp_epoch_in_seconds);

  // send data to cloud every 30 seconds
  // max rate at data.sparkfun.com is 100 in 15 minutes (9 sec)
  if ((millis() - iot_last) > 30000){
    iot_last = millis();
    iot_send_data();
  }

  // update display and serial every 5 seconds
  if (millis() - display_last > 5000) {
      display_last = millis();
      oled_roost();
      serial_roost();
  }

  // let the web server do its thing every iteration
  web_server.handleClient();
}
// =============================================================================
