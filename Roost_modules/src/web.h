// -----------------------------------------------------------------------------
// Web output:  code for dealing with internet for the "Roost!" project
//              requires ESP8266 WiFi libraries
//              https://github.com/esp8266/Arduino/*
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Web server
ESP8266WebServer web_server(80);

// web blink pattern
void web_setup();
void web_blink();
void web_handle_root();
void web_handle_404();
void web_handle_roost();

