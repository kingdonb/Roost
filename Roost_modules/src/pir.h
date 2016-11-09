// -----------------------------------------------------------------------------
// Passive Ifra-Red (PIR) control: code for dealing with the HC-SR501 PIR Motion
//                                 sensor for the "Roost!" project
//

// put the PIR in pin 13
#define PIRPIN 13

unsigned long pir_last_motion;
bool pir_motion;

void pir_setup();
void pir_chk_motion();

