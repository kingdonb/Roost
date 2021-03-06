// =============================================================================
// File: Roost_lab5.ino LEDs, serial, basic Wifi, web output, OLED screen, NTP
// Desc: Roost! An open source implementaion of a temperature and motion 
//       monitoring station based on an ESP8266 with temperature, humidity,
//       motion and distance sensors.
//
//       This code is in the public domain
// =============================================================================

// -----------------------------------------------------------------------------
// LED control:  code for dealing with the LEDs for the "Roost!" project
//

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

// -----------------------------------------------------------------------------
// Network time protocol: code for dealing with NTP for the "Roost!" project
//                        shamelessly adapted from NTPClient example in ESP8266 library 
//                        requires ESP8266 WiFi libraries
//                        https://github.com/esp8266/Arduino
//
#include <WiFiUdp.h>

unsigned int  ntp_local_port = 2390;              // local port to listen for UDP packets
IPAddress     ntp_server_ip;                      // time.nist.gov NTP server address
const char*   ntp_server_name = "time.nist.gov";  //
const int     NTP_PACKET_SIZE = 48;               // NTP time stamp is in the first 48 bytes of the message
byte          ntp_packet_buffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
long          ntp_epoch_in_seconds = 0;           // unix time
long          ntp_received_epoch = 0;             // unix time upon last receive from NTP server
long          ntp_received_millis = 0;            // millis() upon last receive from NTP server
bool          ntp_packet_received = false;        // ntp request state
char*         ntp_hms = "00:00:00";               // HH:MM:SS

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

// convert epoch in seconds to HH:MM:SS
char* ntp_epoch2hms(unsigned long epoch)
{
  char* hms = "00:00:00";
  
  // print the hour, minute and second:
  unsigned long hh=((epoch % 86400L) / 3600);
  unsigned long mm=((epoch % 3600) / 60);
  unsigned long ss=(epoch % 60);
    
  sprintf(hms, "%02d:%02d:%02d", hh, mm, ss);
  return(hms);
}

void ntp_setup()
{
  Serial.println("Starting UDP");
  udp.begin(ntp_local_port);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}

void ntp_send_request()
{
  // get a random server from the pool
  WiFi.hostByName(ntp_server_name, ntp_server_ip);
  
  // send an NTP packet to a time server
  ntp_send_packet();
}

void ntp_read_response()
{
  int cb = udp.parsePacket();
  if (!cb) {
    ntp_packet_received = false;  
    // Serial.println("NTP no packet yet");
    
  } else {
    ntp_packet_received = true;
    Serial.print("NTP packet received, length=");
    Serial.println(cb);
    
    // We've received a packet, read the data from it
    udp.read(ntp_packet_buffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(ntp_packet_buffer[40], ntp_packet_buffer[41]);
    unsigned long lowWord = word(ntp_packet_buffer[42], ntp_packet_buffer[43]);

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    ntp_epoch_in_seconds = secsSince1900 - seventyYears;

    // hold on to epoch and processor time at NTP receive
    ntp_received_epoch = ntp_epoch_in_seconds;
    ntp_received_millis = millis();

    // print Unix time:
    Serial.println(ntp_epoch_in_seconds);
    led_blink(LED_BOTH, 100);
    ntp_hms = ntp_epoch2hms(ntp_epoch_in_seconds);
  }
}

// send an NTP request to the time server at the given address
unsigned long ntp_send_packet()
{
  Serial.println("NTP packet sent...");
  // set all bytes in the buffer to 0
  memset(ntp_packet_buffer, 0, NTP_PACKET_SIZE);
  
  // Initialize values needed to form NTP request
  ntp_packet_buffer[0] = 0b11100011;   // LI, Version, Mode
  ntp_packet_buffer[1] = 0;            // Stratum, or type of clock
  ntp_packet_buffer[2] = 6;            // Polling Interval
  ntp_packet_buffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntp_packet_buffer[12]  = 49;
  ntp_packet_buffer[13]  = 0x4E;
  ntp_packet_buffer[14]  = 49;
  ntp_packet_buffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp
  // NTP requests are to port 123
  udp.beginPacket(ntp_server_ip, 123);
  udp.write(ntp_packet_buffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// -----------------------------------------------------------------------------
// Serial output: code for dealing with serial output for the "Roost!" project
//  

// ESP native speed
#define SERIAL_BAUD 74880

void serial_setup(){
  Serial.begin(SERIAL_BAUD);
  Serial.println("Roost initializing!");
}

void serial_roost() {
  Serial.println("------------------------------");
  
  Serial.print("WiFi address: ");
  Serial.println(wifi_ipaddr);
  
  Serial.print("current epoch: ");
  Serial.println(ntp_epoch_in_seconds);

  Serial.print("GMT: ");
  Serial.println(ntp_hms);
  
  Serial.println("------------------------------");
}

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

// -----------------------------------------------------------------------------
// OLED control: code for dealing with oled for the "Roost!" project
//               uses library by Daniel Eichhorn downloaded from github
//               https://github.com/squix78/esp8266-oled-ssd1306
//
#include "SSD1306.h"
#include "SSD1306Brzo.h"
#include "Liberation_Mono.h"
// #include "images.h"

// Initialize the OLED display using brzo_i2c
// D2 -> SDA
// D14 -> SCL
SSD1306Brzo display(0x3c, 2, 14);
int counter = 0;

// -------------------------------------
// initialize the oled
// -------------------------------------
void oled_setup(){
  display.init();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Liberation_Mono_10); // 21 characters per line

  display.flipScreenVertically(); // optional, puts the headers at the top of the screen
  display.drawString(0, 0, "Cock-a-doodle-doo!");
  display.display();
}

// -------------------------------------
// display roost data on the oled
// -------------------------------------
void oled_roost(){
  char ls[22] = {};                     // line string

  Serial.println("refreshing OLED");

  display.clear();
  
  // ip address on line 1
  display.drawString(0, 0, wifi_ipaddr);

  // time on line 1
  sprintf(ls, "GMT %s", ntp_hms);
  display.drawString(0, 10, ls);
  display.display();
}

// =============================================================================
void setup() {

  // LEDs off, please
  led_setup();

  // Serial for debugging
  serial_setup();

  // WiFi
  wifi_setup();

  // WWW
  web_setup();

  // OLED
  oled_setup();

  // NTP
  ntp_setup();
}

// =============================================================================
void loop() {
  static unsigned long display_last = 0;
  static unsigned long ntp_last = 0;
  
  // update display and serial every 5 seconds
  if (millis() - display_last > 5000) {
      display_last = millis();

      counter++;
      oled_roost();
      serial_roost();
  }

  // check for ntp response if not received
  if (! ntp_packet_received){ ntp_read_response(); };
  
  // keep time updated, last epoch received plus usec since last epoch received / 1000
  ntp_epoch_in_seconds = ntp_received_epoch + floor((millis() - ntp_received_millis) / 1000);
  ntp_hms = ntp_epoch2hms(ntp_epoch_in_seconds);

  // update time every 60 seconds or when the last request is incomplete
  if ((millis() - ntp_last) > 60000 || (! ntp_packet_received)){
    ntp_last = millis();
    ntp_send_request();
  }

  // let the web server do its thing every iteration
  web_server.handleClient();
}

// =============================================================================
