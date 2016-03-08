#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BOARDLEDS 6
Adafruit_NeoPixel boardStrip = Adafruit_NeoPixel(32, BOARDLEDS, NEO_GRB + NEO_KHZ800);


void setup() {
  boardStrip.setBrightness(50);
  boardStrip.begin();
  boardStrip.show(); // Initialize all pixels to 'off'
}

void loop() {
  rainbowCycle(20);
}


void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< boardStrip.numPixels(); i++) {
      boardStrip.setPixelColor(i, Wheel(((i * 256 / boardStrip.numPixels()) + j) & 255));
    }
    boardStrip.show();
    delay(wait);
  }
}
