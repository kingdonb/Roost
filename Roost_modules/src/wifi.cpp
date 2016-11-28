// -----------------------------------------------------------------------------
// Wifi Control: code for dealing with WiFi for the "Roost!" project
//               requires ESP8266 WiFi libraries
//               https://github.com/esp8266/Arduino
//
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>

#include "led.h"
#include "wifi.h"
#include "ntp.h"
#include "dht.h"
#include "pir.h"
#include "sonic.h"

char wifi_ipaddr[21] = {};

// convert int to dotted quad. thanks stackoverflow.com
void wifi_format_ip(){
  uint32_t ip=WiFi.localIP();
  unsigned char bytes[4];

  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  sprintf(wifi_ipaddr, "IP %d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}

void wifi_setup(){
  //Fire up the wifi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("o");
    led_blink(LED_EXTRA, 50);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  wifi_format_ip();
  Serial.print(" network on IP address: ");
  Serial.println(wifi_ipaddr);
}
