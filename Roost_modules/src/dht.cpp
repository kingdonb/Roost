// -----------------------------------------------------------------------------
// DHT11 Control: Digital humididy and temperature sensor
//       code for dealing with the DHT11 sensor for the "Roost!" project
//       requires DHT_sensor_library from Adafruit
//       assumes Serial is enabled
//       https://github.com/adafruit/dht-sensor-library
//

void dht_clear(){ h=t=f=hic=hif=0.0; }

void dht_setup(){ dht_clear(); }

// -------------------------------------
// take a reading from the DHT11
// -------------------------------------
void dht_read() {
  // shamelessly adapted from DHTtester.ino in Adafruit library examples

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();

  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    h=t=f=hic=hif=0.0;

  } else {
    // Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(f, h);

    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(t, h, false);
  }
}

