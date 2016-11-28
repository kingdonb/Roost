// -----------------------------------------------------------------------------
// SR04 Control: Ultrasonic distance sensor sensor
//       code for dealing with the SR04 sensor for the "Roost!" project
//       NOTE: speed of sound is 29.14 usec per centimeter- per Woframalpha
//
#define sr_trig 0
#define sr_echo 16
long sr_cm;

void sr_setup();
void sr_ping();
