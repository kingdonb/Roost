// -----------------------------------------------------------------------------
// Passive Ifra-Red (PIR) control: code for dealing with the HC-SR501 PIR Motion
//                                 sensor for the "Roost!" project
//

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

