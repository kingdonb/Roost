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

void iot_setup(){
    // client from WiFi client
    Serial.println("using IoT phant client");
}

void iot_send_data(){
    Serial.print("posting to ");
    Serial.println(phantHost);

    if (!client.connect(phantHost, phantPort)) {
      Serial.println("connection failed");
      return;
    }
    phant.add("temp_c",t);
    phant.add("temp_f",f);
    phant.add("humidity",h);
    phant.add("epoch",ntp_epoch_in_seconds);
    phant.add("motion",pir_last_motion);
    phant.add("distance",sr_cm);

    Serial.println("----HTTP POST----");
    Serial.println(phant.queryString());
    client.print(phant.post());

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("client timeout :(");
        client.stop();
        return;
      }
    }

    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
}
