// -----------------------------------------------------------------------------
// Serial output: code for dealing with serial output for the "Roost!" project
//

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

