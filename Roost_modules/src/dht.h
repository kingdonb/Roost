// -----------------------------------------------------------------------------
// DHT11 Control: Digital humididy and temperature sensor
//       code for dealing with the DHT11 sensor for the "Roost!" project
//       requires DHT_sensor_library from Adafruit
//       assumes Serial is enabled
//       https://github.com/adafruit/DHT-sensor-library
//
#include <DHT.h>

// put the DHT22 on pin 12
#define DHTPIN 12
#define DHTTYPE DHT22

// Digital Humidity Temperature Sensor
DHT dht(DHTPIN, DHTTYPE);

// Temperature, humidity and heat index variables
float h=0,t=0,f=0,hic=0,hif=0;

void dht_clear();
void dht_setup();
void dht_read();
