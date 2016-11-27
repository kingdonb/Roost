// -----------------------------------------------------------------------------
// Passive Ifra-Red (PIR) control: code for dealing with the HC-SR501 PIR Motion
//                                 sensor for the "Roost!" project
//

// put the PIR in pin 13
#define PIRPIN 13

long pir_last_motion = 0;
bool pir_motion = false;

void pir_setup();
void pir_chk_motion();
