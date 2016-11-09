// -----------------------------------------------------------------------------
// DHT11 Control: Digital humididy and temperature sensor
//       code for dealing with the DHT11 sensor for the "Roost!" project
//       requires DHT_sensor_library from Adafruit
//       assumes Serial is enabled
//       https://github.com/adafruit/dht-sensor-library
//
#include <DHT.h>

// put the DHT11 on pin 12
#define DHTPIN 12
#define DHTTYPE DHT11

// Temperature, humidity and heat index variables
float h,t,f,hic,hif;

// Digital Humidity Temperature Sensor
DHT dht(DHTPIN, DHTTYPE);

void dht_clear();
void dht_setup();
void dht_read();

