// =============================================================================
// File: Roost_lab1.ino
// Desc: Roost! An open source implementaion of a temperature and motion
//       monitoring station based on an ESP8266 with DHT22 and HC-SR501 sensors.
//
//       This code is in the public domain
// =============================================================================

// -----------------------------------------------------------------------------
// LED control:  code for dealing with the LEDs for the "Roost!" project

// default led on pin 5 (also onboard led)
#define LED_DEFAULT 5
// extra led on pin 4
#define LED_EXTRA 4
// both pins
#define LED_BOTH -1

void led_setup() {
  digitalWrite(LED_DEFAULT, LOW);
  digitalWrite(LED_EXTRA, LOW);

  led_blink(LED_DEFAULT, 50);
  led_blink(LED_EXTRA, 50);
  led_blink(LED_BOTH, 50);
}

// -------------------------------------
// blink the led on pin l for d milliseconds
// -------------------------------------
void led_blink(int l, int d){
  // If led not specified then use default
  if (l != LED_EXTRA && l != LED_BOTH) { l = LED_DEFAULT; }

  // minimum blink is 10 millis
  if (d<10) { d=10; }

  // maximum blink is 5 seconds
  if (d>5000) { d=5000; }

  // blink on
  if (l == LED_BOTH) {
    digitalWrite(LED_DEFAULT, HIGH);
    digitalWrite(LED_EXTRA, HIGH);
  } else {
    digitalWrite(l, HIGH);
  }

  // pause ...
  delay(d);

  // blink off
  if (l == LED_BOTH) {
    digitalWrite(LED_DEFAULT, LOW);
    digitalWrite(LED_EXTRA, LOW);
  } else {
    digitalWrite(l, LOW);
  }
}

// =============================================================================
void setup() {

  // leds off, please
  led_setup();
}

// =============================================================================
void loop() {
  led_blink(LED_DEFAULT, 500);
  delay(500);

  led_blink(LED_EXTRA, 500);
  delay(500);

  led_blink(LED_BOTH, 500);
  delay(500);
}

// =============================================================================
