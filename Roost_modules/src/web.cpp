// -----------------------------------------------------------------------------
// Web output:  code for dealing with internet for the "Roost!" project
//              requires ESP8266 WiFi libraries
//              https://github.com/esp8266/Arduino
//
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Web server
ESP8266WebServer web_server(80);

// web blink pattern
void web_blink(){
  led_blink(LED_DEFAULT, 10);
  delay(50);
  led_blink(LED_DEFAULT, 10);
}

// root web page
void web_handle_root() {
  web_server.send(200, "text/plain", "Try /roost");
  web_blink();
}

// 404
void web_handle_404(){
  String message = "404 File Not Found\n\n";
  message += "URI: ";
  message += wifi_ipaddr;
  message += web_server.uri();
  message += "\nMethod: ";
  message += (web_server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += web_server.args();
  message += "\n";
  for (uint8_t i=0; i< web_server.args(); i++){
    message += " " + web_server.argName(i) + ": " + web_server.arg(i) + "\n";
  }
  web_server.send(404, "text/plain", message);
  web_blink();
}

// display roost data on the web
void web_handle_roost(){
  char x[64] = {};
  String message = "Roost!\n";

  sprintf(x, "\nCurrent epoch: %d", ntp_epoch_in_seconds);
  message += x;

  // GMT
  message += "\nGMT: ";
  message += ntp_hms;

  sprintf(x, "\nLast motion: %d", pir_last_motion);
  message += x;

  // whoa! Arduino did not implement %f in sprintf
  // this is one of two workarounds (better because
  // it handles negative numbers)
  strcpy(x, "\nTemp C: ");
  dtostrf(t, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nTemp F: ");
  dtostrf(f, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nHumidity %");
  dtostrf(h, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nHeat Index C: ");
  dtostrf(hic, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nHeat Index F: ");
  dtostrf(hif, 2, 2, &x[strlen(x)]);
  message += x;

  // Last motion
  message += "\nMotion @ ";
  message += ntp_epoch2hms(pir_last_motion);

  // ultrasonic distance
  strcpy(x, "\ndistance ");
  dtostrf(sr_cm, 2, 2, &x[strlen(x)]);
  message += x;
  message += " cm";

  web_server.send(200, "text/plain", message);
  web_blink();
}

void web_setup(){
  // base page
  web_server.on("/", web_handle_root);

  // roost page
  web_server.on("/roost", web_handle_roost);

  // no page at all
  web_server.onNotFound(web_handle_404);

  // Start the web
  web_server.begin();
  Serial.println("HTTP server started");
}
