// -----------------------------------------------------------------------------
// SR04 Control: Ultrasonic distance sensor sensor
//       code for dealing with the SR04 sensor for the "Roost!" project
//       NOTE: speed of sound is 29.14 usec per centimeter- per Woframalpha
//
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

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
