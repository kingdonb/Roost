// -----------------------------------------------------------------------------
// OLED control: code for dealing with oled for the "Roost!" project
//               uses library by Daniel Eichhorn downloaded from github
//               https://github.com/squix78/esp8266-oled-ssd1306
//
#include "SSD1306.h"
#include "SSD1306Brzo.h"
// #include "images.h"

// Initialize the OLED display using brzo_i2c
// D2 -> SDA
// D14 -> SCL
SSD1306Brzo display(0x3c, 2, 14);

void oled_setup();
void oled_roost();

