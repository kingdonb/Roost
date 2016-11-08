// -----------------------------------------------------------------------------
// Wifi Control: code for dealing with WiFi for the "Roost!" project
//               requires ESP8266 WiFi libraries
//               https://github.com/esp8266/Arduino/*
//
#include <ESP8266WiFi.h>

// my home network
// -------------------------------------
const char* ssid = "Lincoln Manor";
const char* password = "...---... sos ...---...";

// Notre Dame public WiFi
// -------------------------------------
// const char* ssid = "ND-guest";
// const char* password = "";

// Roost class network
// -------------------------------------
// const char* ssid = "XXXXXXXXXX";
// const char* password = "XXXXXXXXXX";

char wifi_ipaddr[21] = {};

void wifi_format_ip();
void wifi_setup();

