// -----------------------------------------------------------------------------
// IoT output:  code for dealing with Phant for the "Roost!" project
//              requires Thingspeak libraries
//
#include <ESP8266WiFi.h>
#include "Phant.h"

const char * phantHost = "data.sparkfun.com";
const char * phantPublicKey = "XXXXXXXXXXXXXXXXXXXX";
const char * phantPrivateKey = "XXXXXXXXXXXXXXXXXXXX";
const int    phantPort = 80;

Phant phant("data.sparkfun.com", phantPublicKey, phantPrivateKey);
WiFiClient  client;

void iot_setup();
void iot_send_data();
