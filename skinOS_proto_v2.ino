#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

#define PANELPIN 6
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, PANELPIN, NEO_GRB + NEO_KHZ800);

CapacitiveSensor   cs_1 = CapacitiveSensor(5,12); // 1M resistor between pins 5 & 12, pin 12 is sensor pin
CapacitiveSensor   cs_2 = CapacitiveSensor(9,13); // 1M resistor between pins 5 & 12, pin 12 is sensor pin


int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25};
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};


void setup() {
  Serial.begin(9600);
    
  panelStrip.setBrightness(2);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  //attachInterrupt(digitalPinToInterrupt(coilPin), panelOn, LOW);
}

void loop() {
  long cap1 =  cs_1.capacitiveSensor(30);
  long cap2 =  cs_2.capacitiveSensor(30);
  Serial.print(cap1);
  Serial.print(" : ");
  Serial.println(cap2);
  delay(10);
}

void launchPanels() {
  panelOn(panelLeft);
  panelOn(panelCenter);
  panelOn(panelRight);
}

void panelOn(int *panel) { 
  int colorCounter = 0;
  while (colorCounter <= 127) {
    for(int i = 0; i < 8; i++) {
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter,colorCounter,colorCounter));
        panelStrip.show();
        colorCounter += 3;
        delay(10);
    }
  } 
}




