// -----------------------------------------------------------------------------
// Serial output: code for dealing with serial output for the "Roost!" project
//

#define SERIAL_BAUD 74880               // ESP native speed

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

