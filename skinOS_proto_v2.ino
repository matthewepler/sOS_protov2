#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

#define PANELPIN 6
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, PANELPIN, NEO_GRB + NEO_KHZ800);
int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25};
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};

CapacitiveSensor   cs_1 = CapacitiveSensor(5, 12); // 1M resistor between pins 5 & 12, pin 12 is sensor pin
CapacitiveSensor   cs_2 = CapacitiveSensor(9, 13); // 1M resistor between pins 5 & 12, pin 12 is sensor pin
boolean touch;
int offCounter;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);

  panelStrip.setBrightness(2);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  //attachInterrupt(digitalPinToInterrupt(coilPin), panelOn, LOW);
}

void loop() {
  checkCapSensors(); // has 10ms delay

  if (touch) {
    analogWrite(10, 122);
  } else {
    analogWrite(10, 0);
  }

  Serial.println(touch);
}

void launchPanels() {
  panelSet(panelLeft);
  panelSet(panelCenter);
  panelSet(panelRight);
}

void panelSet(int *panel) {
  int colorCounter = 0;
  while (colorCounter <= 127) {
    for (int i = 0; i < 8; i++) {
      panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
      panelStrip.show();
      colorCounter += 3;
      delay(10);
    }
  }
}

void checkCapSensors() {
  long cap1 =  cs_1.capacitiveSensor(30);
  long cap2 =  cs_2.capacitiveSensor(30);
  if ( cap1 > 200 && cap2 > 200) {
    if (!touch) {
      launchPanels();
      touch = true;
    }
  } else {
    if(touch) {
      if (offCounter > 100) {
        offCounter = 0;
        touch = false;
      } else {
        offCounter++;
      }
    }
  }
  delay(10);
}




