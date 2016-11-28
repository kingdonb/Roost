// -----------------------------------------------------------------------------
// Network time protocol: code for dealing with NTP for the "Roost!" project
//                        shamelessly adapted from NTPClient example in ESP8266 library
//                        requires ESP8266 WiFi libraries
//                        https://github.com/esp8266/Arduino
//
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "led.h"

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
