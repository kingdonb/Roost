// =============================================================================
// File: roost.ino
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
float h=0,t=0,f=0,hic=0,hif=0;

void dht_clear(){ h=t=f=hic=hif=0; }

void dht_setup(){ dht_clear(); }

// -------------------------------------
// take a reading from the DHT22
// -------------------------------------
void dht_read() {
  // shamelessly adapted from DHTtester.ino in Adafruit library examples
  dht_clear();
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();

  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    // Failed to read from DHT sensor
    led_blink(LED_DEFAULT, 50);
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
// Passive Ifra-Red (PIR) control: code for dealing with the HC-SR501 PIR Motion 
//                                 sensor for the "Roost!" project
//

// put the PIR in pin 13
#define PIRPIN 13

long pir_last_motion = 0;
bool pir_motion = false;

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
      led_blink(LED_BOTH, 100);
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
// SR04 Control: Ultrasonic distance sensor sensor
//       code for dealing with the SR04 sensor for the "Roost!" project
//       NOTE: speed of sound is 29.14 usec per centimeter- per Woframalpha
//
#define sr_trig 0
#define sr_echo 16
long sr_cm = 0;

void sr_setup() {
  // define pins and set echo pin LOW
  Serial.println("Set up ultrasonic sensor");

  pinMode(sr_trig, OUTPUT);
  pinMode(sr_echo, INPUT);
  digitalWrite(sr_trig, LOW);
}

void sr_ping(){
  // trigger a ping with a 10 usec high pulse

  digitalWrite(sr_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sr_trig, LOW);
  
  int usec = pulseIn(sr_echo, HIGH);
  Serial.print("...oooOOO))) usec ");
  Serial.println(usec);

  sr_cm = (usec/2) / 29.14;

  if (sr_cm < 1){ sr_cm = 1; }
  if (sr_cm > 999){ sr_cm = 999; }
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
  
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.println(" *F");
  
  Serial.print("Humidity %");
  Serial.println(h);

  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F\t");

  Serial.print("last motion: ");
  Serial.println(ntp_epoch2hms(pir_last_motion));  
  
  Serial.print("last distance: ");
  Serial.println(sr_cm);
  Serial.println("------------------------------");
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
  
  // ip address on line 1
  display.drawString(0, 0, wifi_ipaddr);

  // time on line 2
  sprintf(ls, "GMT %s", ntp_hms);
  display.drawString(0, 10, ls);
  
  // temperature on line 2
  dtostrf(t, 2, 2, &ts[0]);
  dtostrf(f, 2, 2, &fs[0]);
  sprintf(ls, "Temp c %s f %s", ts, fs);
  display.drawString(0, 20, ls);
  
  // humidity on line 4
  dtostrf(h, 2, 2, &hs[0]);
  sprintf(ls,"Humidity %%%s", hs);
  display.drawString(0, 30, ls);

  // heat index on line 5
  dtostrf(hic, 2, 2, &ts[0]);
  dtostrf(hif, 2, 2, &fs[0]);
  sprintf(ls, "Indx c %s f %s", ts, fs);
  display.drawString(0, 40, ls);

  // last motion on line 6
  sprintf(ls, "Motion @ %s", ntp_epoch2hms(pir_last_motion));
  display.drawString(0, 50, ls);
  
  display.display();
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

// =============================================================================
//
// =============================================================================
void setup() {

  // leds off, please
  led_setup();
  
  // serial for debugging
  serial_setup();
  
  // WiFi
  wifi_setup();
  
  // digital humididy temperature
  dht_setup();

  // PIR
  pir_setup();

  // SR04
  sr_setup();

  // OLED
  oled_setup();
 
  // NTP
  ntp_setup();
  
  // WWW
  web_setup();

  // IoT
  iot_setup();
}

// =============================================================================
void loop() {
  static int display_last = 0;
  static int iot_last = 0;
  static int sr_last = 0;

  // get humidity and temp
  dht_clear();
  dht_read();

  // check for motion
  pir_chk_motion();

  // check proximity every 1 second using ultrasonic sensor
  if (millis() - sr_last > 1000) {
    sr_last = millis();
    sr_ping();
  }

  // request NTP time every 60 seconds
  if ((millis() - ntp_received_millis) > 60000){
    ntp_send_request();
  }

  // check for ntp response if not received
  if (! ntp_packet_received){ ntp_read_response(); };
  
  // keep time updated, last epoch received plus usec since last epoch received / 1000
  ntp_epoch_in_seconds = ntp_received_epoch + floor((millis() - ntp_received_millis) / 1000);
  ntp_hms = ntp_epoch2hms(ntp_epoch_in_seconds);

  // send data to cloud every 30 seconds 
  // max rate at data.sparkfun.com is 100 in 15 minutes (9 sec)
  if ((millis() - iot_last) > 30000){
    iot_last = millis();
    iot_send_data();
  }

  // update display and serial every 5 seconds
  if (millis() - display_last > 5000) {
      display_last = millis();      
      oled_roost();
      serial_roost();
  }

  // let the web server do its thing every iteration
  web_server.handleClient();
}

// =============================================================================

