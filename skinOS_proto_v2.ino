#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

const int coilPin = 5;
int panelDelay = 500;

#define BOARDLEDS 6
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, BOARDLEDS, NEO_GRB + NEO_KHZ800);

int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25};
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};


void setup() {
  Serial.begin(9600);

  panelStrip.setBrightness(50);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  //attachInterrupt(digitalPinToInterrupt(coilPin), panelOn, LOW);
  
  panelOn(panelLeft);
  //delay(panelDelay);
  panelOn(panelCenter);
  //delay(panelDelay);
  panelOn(panelRight);
}

void loop() {
  //Serial.println(digitalRead(5));
  
}

void panelOn(int *panel) { 
  int colorCounter = 0;
  while (colorCounter <= 255) {
    for(int i = 0; i < 8; i++) {
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter,colorCounter,colorCounter));
        panelStrip.show();
        colorCounter += 1;
        delay(10);
    }
  } 
}




