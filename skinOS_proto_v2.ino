#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

const int coilPin = 5;

#define BOARDLEDS 6
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, BOARDLEDS, NEO_GRB + NEO_KHZ800);

int panelLeft[]   = {1,2,9,10,17,18,25,26};
int panelCenter[] = {4,5,12,13,20,21,28,29};
int panelRight[]  = {7,8,15,16,23,24,31,32};


void setup() {
  Serial.begin(9600);
  
  panelStrip.setBrightness(50);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  attachInterrupt(digitalPinToInterrupt(coilPin), panelOn, LOW);
}

void loop() {
  Serial.println(digitalRead(5));
}

void panelOn() {
  Serial.println("panelOn()");
}

