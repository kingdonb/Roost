// -----------------------------------------------------------------------------
// IoT output:  code for dealing with Phant for the "Roost!" project
//              requires Thingspeak libraries
//
#include "Phant.h"
const char * phantHost = "data.sparkfun.com";
const char * phantPublicKey = "VG6Ww5a4DxsdJyqpLVz6";
const char * phantPrivateKey = "9Y1qEzjeydi2o7vXx8MY";
const int    phantPort = 80;

Phant phant("data.sparkfun.com", phantPublicKey, phantPrivateKey);
WiFiClient  client;

void iot_setup();
void iot_send_data();
