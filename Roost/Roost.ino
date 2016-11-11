// =============================================================================
// File: roost.ino
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
  digitalWrite(LED_DEFAULT, LOW);
  digitalWrite(LED_EXTRA, LOW);

  led_blink(LED_DEFAULT, 500);
  led_blink(LED_EXTRA, 500);
  led_blink(LED_BOTH, 500);
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
// DHT22 Control: Digital humididy and temperature sensor
//       code for dealing with the DHT22 sensor for the "Roost!" project
//       requires DHT_sensor_library from Adafruit
//       assumes Serial is enabled
//       https://github.com/adafruit/Adafruit_Sensor
//       https://github.com/adafruit/DHT-sensor-library
//
#include <DHT.h>

// put the DHT22 on pin 12
#define DHTPIN 12
#define DHTTYPE DHT22

// Digital Humidity Temperature Sensor
DHT dht(DHTPIN, DHTTYPE);

// Temperature, humidity and heat index variables
float h,t,f,hic,hif;

void dht_clear(){ h=t=f=hic=hif=0; }

void dht_setup(){ dht_clear(); }

// -------------------------------------
// take a reading from the DHT22
// -------------------------------------
void dht_read() {
  // shamelessly adapted from DHTtester.ino in Adafruit library examples

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();

  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    h=t=f=hic=hif=0;

  } else {
    // Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(f, h);

    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(t, h, false);
  }
}

// -----------------------------------------------------------------------------
// Network time protocol: code for dealing with NTP for the "Roost!" project
//                        shamelessly adapted from NTPClient example in ESP8266 library 
//                        requires ESP8266 WiFi libraries
//                        https://github.com/esp8266/Arduino
//
#include <WiFiUdp.h>

unsigned int ntp_local_port = 2390;             // local port to listen for UDP packets
IPAddress ntp_server_ip;                        // time.nist.gov NTP server address
const char* ntp_server_name = "time.nist.gov";  //
const int NTP_PACKET_SIZE = 48;                 // NTP time stamp is in the first 48 bytes of the message
byte ntp_packet_buffer[ NTP_PACKET_SIZE];       // buffer to hold incoming and outgoing packets
unsigned long ntp_epoch_in_seconds;             // unix time
bool ntp_packet_received;                       // ntp request state
char* ntp_gmt = "00:00:00";                     // GMT HH:MM:SS

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

char* ntp_epoch2gmt(unsigned long epoch)
{
  char* gmt = "00:00:00";
  
  // UTC is the time at Greenwich Meridian (GMT)
  // print the hour, minute and second:
  int hh=((ntp_epoch_in_seconds % 86400L) / 3600);
  int mm=((ntp_epoch_in_seconds % 3600) / 60);
  int ss=(ntp_epoch_in_seconds % 60);
    
  sprintf(gmt, "%2d:%2d:%2d", hh, mm, ss);
  Serial.print("The UTC time is: ");
  Serial.println(gmt);

  return(gmt);
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
  //get a random server from the pool
  WiFi.hostByName(ntp_server_name, ntp_server_ip); 

  ntp_send_packet(); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  } else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(ntp_packet_buffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
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
    unsigned long ntp_epoch_in_seconds = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(ntp_epoch_in_seconds);

    ntp_gmt = ntp_epoch2gmt(ntp_epoch_in_seconds);
  }
}

// send an NTP request to the time server at the given address
unsigned long ntp_send_packet()
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(ntp_packet_buffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  ntp_packet_buffer[0] = 0b11100011;   // LI, Version, Mode
  ntp_packet_buffer[1] = 0;     // Stratum, or type of clock
  ntp_packet_buffer[2] = 6;     // Polling Interval
  ntp_packet_buffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntp_packet_buffer[12]  = 49;
  ntp_packet_buffer[13]  = 0x4E;
  ntp_packet_buffer[14]  = 49;
  ntp_packet_buffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(ntp_server_ip, 123); //NTP requests are to port 123
  udp.write(ntp_packet_buffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// -----------------------------------------------------------------------------
// Passive Ifra-Red (PIR) control: code for dealing with the HC-SR501 PIR Motion 
//                                 sensor for the "Roost!" project
//

// put the PIR in pin 13
#define PIRPIN 13

unsigned long pir_last_motion;
bool pir_motion;

void pir_setup() {
  pinMode(PIRPIN, INPUT);
}

// -------------------------------------
// take a reading from the PIR sensor
// -------------------------------------
void pir_chk_motion(){
  // check for motion
  if (digitalRead(PIRPIN) == HIGH) {
    if (! pir_motion) {
      // something moved
      led_blink(LED_DEFAULT, 100);
      pir_motion = true;
      pir_last_motion = ntp_epoch_in_seconds;
    }
  } else {
    if (pir_motion) {
      // something stopped moving
      pir_motion = false;
    }
  }
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
  Serial.print("NTP epoch: ");
  Serial.print(ntp_epoch_in_seconds);

  Serial.print(" \tHumidity: ");
  Serial.print(h);

  Serial.print(" % \tTemperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F");

  Serial.print(" \tHeat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.print(" *F\t");

  Serial.print("last motion: ");
  Serial.println(pir_last_motion);
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
  char hs[6] = "00.00";                 // humidity
  char ts[6] = "00.00";                 // temp Celsius
  char fs[6] = "00.00";                 // temp Fahrenheit
  char ls[22] = {};                     // line string

  Serial.println("refreshing OLED");

  display.clear();
  // ip address on line one
  display.drawString(0, 0, wifi_ipaddr);

  // humidity on line two
  dtostrf(h, 2, 2, &hs[0]);
  sprintf(ls,"Humidity %%%s", hs);
  display.drawString(0, 10, ls);

  // temperature on line three
  dtostrf(t, 2, 2, &ts[0]);
  dtostrf(f, 2, 2, &fs[0]);
  sprintf(ls, "Temp c %s f %s", ts, fs);
  display.drawString(0, 20, ls);

  // heat index on line four
  dtostrf(hic, 2, 2, &ts[0]);
  dtostrf(hif, 2, 2, &fs[0]);
  sprintf(ls, "Indx c %s f %s", ts, fs);
  display.drawString(0, 30, ls);

  // time on line 5
  sprintf(ls, "GMT Time %s", ntp_gmt);
  display.drawString(0, 40, ls);

  // last motion on line six
  sprintf(ls, "Motion @ %s", ntp_epoch2gmt(pir_last_motion));
  display.drawString(0, 50, ls);
  
  display.display();
}

// -----------------------------------------------------------------------------
// Web output:  code for dealing with internet for the "Roost!" project
//              requires ESP8266 WiFi libraries
//              https://github.com/esp8266/Arduino
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

// display roost data on the web
void web_handle_roost(){
  char x[64] = {};
  String message = "Roost!\n";

  sprintf(x, "\nNTP Time: %d", ntp_epoch_in_seconds);
  message += x;

  sprintf(x, "\nLast motion: %d", pir_last_motion);
  message += x;

  // whoa! Arduino did not implement %f in sprintf
  // this is one of two workarounds (better because
  // it handles negative numbers)
  strcpy(x, "\nHumidity %");
  dtostrf(h, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nTemp C: ");
  dtostrf(t, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nTemp F: ");
  dtostrf(f, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nHeat Index C: ");
  dtostrf(hic, 2, 2, &x[strlen(x)]);
  message += x;

  strcpy(x, "\nHeat Index F: ");
  dtostrf(hif, 2, 2, &x[strlen(x)]);
  message += x;

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
// IoT output:  code for dealing with Thingspeak for the "Roost!" project
//              requires Thingspeak libraries
//
#include "ThingSpeak.h"
unsigned long myChannelNumber = 00000;
const char * myWriteAPIKey = "xxxxxxxxxxxxxxxxx";
int thingFoo;

void iot_setup(){
    // client from WiFi client
    WiFiClient  client;
    ThingSpeak.begin(client);
}

void iot_send_data(){
    ThingSpeak.setField(1,thingFoo);
    ThingSpeak.setField(2,thingFoo);
    ThingSpeak.setField(3,thingFoo);
    ThingSpeak.setField(4,thingFoo);
    ThingSpeak.setField(5,thingFoo);

    // Write the fields that you've set all at once.
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  

    // delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}
// =============================================================================
// =============================================================================
void setup() {

  // leds off, please
  led_setup();
  
  // Serial for debugging
  serial_setup();
  
  // WiFi
  wifi_setup();
  
  // digital humididy temperature
  dht_setup();

  // PIR
  pir_setup();

  // OLED
  oled_setup();
 
  // NTP
  ntp_setup();
  
  // WWW
  web_setup();
}
// =============================================================================
void loop() {
  static int display_last = 0;
  static int ntp_last = 0;
  
  // clear the roost
  dht_clear();

  // get humidity and temp
  dht_read();

  // check for motion
  pir_chk_motion();

  // keep time updated
  ntp_epoch_in_seconds += (millis() / 1000);
  
  // update display and serial every 5 seconds
  if (millis() - display_last > 5000) {
      display_last = millis();      
      oled_roost();
      serial_roost();
  }

  // update time every 60 seconds or when the last request is incomplete
  if ((millis() - ntp_last) > 60000 || (! ntp_packet_received)){
    ntp_last = millis();
    ntp_send_request();
  }

  // let the web server do its thing every iteration
  web_server.handleClient();
}
// =============================================================================

