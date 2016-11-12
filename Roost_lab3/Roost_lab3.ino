// =============================================================================
// File: Roost_lab3.ino
// Desc: Roost! An open source implementaion of a temperature and motion
//       monitoring station based on an ESP8266 with DHT22 and HC-SR501 sensors.
//
//       This code is in the public domain
// =============================================================================

// -----------------------------------------------------------------------------
// LED control:  code for dealing with the LEDs for the "Roost!" project

// default led on pin 5 (also onboard led)
#define LED_DEFAULT 5
// extra led on pin 4
#define LED_EXTRA 4
// both pins
#define LED_BOTH -1

void led_setup() {
  pinMode(LED_DEFAULT, OUTPUT);
  pinMode(LED_EXTRA, OUTPUT);

  digitalWrite(LED_DEFAULT, LOW);
  digitalWrite(LED_EXTRA, LOW);

  led_blink(LED_DEFAULT, 50);
  led_blink(LED_EXTRA, 50);
  led_blink(LED_BOTH, 50);
}

// -------------------------------------
// blink the led on pin l for d milliseconds
// -------------------------------------
void led_blink(int l, int d){
  // If led not specified then use default
  if (l != LED_EXTRA && l != LED_BOTH) { l = LED_DEFAULT; }

  // minimum blink is 10 millis
  if (d<10) { d=10; }

  // maximum blink is 5 seconds
  if (d>5000) { d=5000; }

  // blink on
  if (l == LED_BOTH) {
    digitalWrite(LED_DEFAULT, HIGH);
    digitalWrite(LED_EXTRA, HIGH);
  } else {
    digitalWrite(l, HIGH);
  }

  // pause ...
  delay(d);

  // blink off
  if (l == LED_BOTH) {
    digitalWrite(LED_DEFAULT, LOW);
    digitalWrite(LED_EXTRA, LOW);
  } else {
    digitalWrite(l, LOW);
  }
}

// -----------------------------------------------------------------------------
// Wifi Control: code for dealing with WiFi for the "Roost!" project
//               requires ESP8266 WiFi libraries
//               https://github.com/esp8266/Arduino
//
#include <ESP8266WiFi.h>

// Notre Dame public WiFi
// -------------------------------------
const char* ssid = "ND-guest";
const char* password = "";

// Roost class network
// -------------------------------------
// const char* ssid = "XXXXXXXXXX";
// const char* password = "XXXXXXXXXX";

char wifi_ipaddr[21] = {};

// convert int to dotted quad. thanks stackoverflow.com
void wifi_format_ip(){
  uint32_t ip=WiFi.localIP();
  unsigned char bytes[4];

  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  sprintf(wifi_ipaddr, "%d.%d.%d.%d/roost", bytes[0], bytes[1], bytes[2], bytes[3]);
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
  Serial.print(" network on IP address: ");
  Serial.println(WiFi.localIP());

  wifi_format_ip();
  Serial.println(wifi_ipaddr);
}

// -----------------------------------------------------------------------------
// Serial output: code for dealing with serial output for the "Roost!" project
//
#define SERIAL_BAUD 74880               // ESP native speed

void serial_setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("Roost initializing!");
}

void serial_roost() {
  Serial.println(wifi_ipaddr);

}

// -----------------------------------------------------------------------------
// Web output:  code for dealing with internet for the "Roost!" project
//              requires ESP8266 WiFi libraries
//              https://github.com/esp8266/Arduino/*
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Web server
ESP8266WebServer web_server(80);

// web blink pattern
void web_blink(){
  led_blink(LED_EXTRA, 10);
  delay(50); 
  led_blink(LED_EXTRA, 10);
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

// display Hello, Roost! on the web
void web_handle_roost(){
  char x[64] = {};
  String message = "Hello, Roost!\n";

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

// =============================================================================
void setup() {

  // LEDs off, please
  led_setup();

  // Serial for debugging
  serial_setup();

  // WiFi
  wifi_setup();

  // Web
  web_setup();
}

// =============================================================================
void loop() {
  led_blink(LED_DEFAULT, 500);
  delay(500);

  led_blink(LED_EXTRA, 500);
  delay(500);

  led_blink(LED_BOTH, 500);
  delay(500);

  // let the web server do its thing every iteration
  web_server.handleClient();
}

// =============================================================================
