// -----------------------------------------------------------------------------
// Wifi Control: code for dealing with WiFi for the "Roost!" project
//               requires ESP8266 WiFi libraries
//               https://github.com/esp8266/Arduino/*
//
#include <ESP8266WiFi.h>

// Notre Dame public WiFi
// -------------------------------------
const char* ssid = "ND-guest";
const char* password = "";

char wifi_ipaddr[21];

// convert int to dotted quad. thanks stackoverflow.com
void wifi_format_ip();
void wifi_setup();
