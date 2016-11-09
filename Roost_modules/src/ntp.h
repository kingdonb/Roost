// -----------------------------------------------------------------------------
// Network time protocol: code for dealing with NTP for the "Roost!" project
//                        shamelessly adapted from NTPClient example in ESP8266 library
//                        requires ESP8266 WiFi libraries
//                        https://github.com/esp8266/Arduino/*
//
#include <WiFiUdp.h>

unsigned int ntp_local_port = 2390;             // local port to listen for UDP packets
IPAddress ntp_server_ip;                        // time.nist.gov NTP server address
const char* ntp_server_name = "time.nist.gov";  //
const int NTP_PACKET_SIZE = 48;                 // NTP time stamp is in the first 48 bytes of the message
byte ntp_packet_buffer[ NTP_PACKET_SIZE];       // buffer to hold incoming and outgoing packets
unsigned long ntp_epoch_in_seconds;             // unix time
bool ntp_packet_received;                       // ntp request state

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void ntp_setup();
void ntp_send_request();
unsigned long ntp_send_packet();

