// -----------------------------------------------------------------------------
// Network time protocol: code for dealing with NTP for the "Roost!" project
//                        shamelessly adapted from NTPClient example in ESP8266 library
//                        requires ESP8266 WiFi libraries
//                        https://github.com/esp8266/Arduino/*
//

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

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((ntp_epoch_in_seconds  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((ntp_epoch_in_seconds % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((ntp_epoch_in_seconds  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (ntp_epoch_in_seconds % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(ntp_epoch_in_seconds % 60); // print the second
  }
  // wait ten seconds before asking for the time again
  delay(10000);
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

