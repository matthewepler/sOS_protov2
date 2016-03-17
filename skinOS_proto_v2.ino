#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <EnableInterrupt.h>

#define PANELPIN 6
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, PANELPIN, NEO_GRB + NEO_KHZ800);
int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25};
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};

CapacitiveSensor   cs_1 = CapacitiveSensor(5, 12); // 1M resistor between pins 5 & 12, pin 12 is sensor pin
CapacitiveSensor   cs_2 = CapacitiveSensor(9, 13); // 1M resistor between pins 5 & 12, pin 12 is sensor pin
boolean touch;
int offCounter;

const int coilPin = 11;
boolean docked;


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);

  panelStrip.setBrightness(2);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  pinMode(coilPin, INPUT_PULLUP);  // See http://arduino.cc/en/Tutorial/DigitalPins
  enableInterrupt(coilPin, dockChange, CHANGE);}

void loop() {
  checkCapSensors(); // has 10ms delay
  
  if (touch) {
    analogWrite(10, 122);
  } else {
    analogWrite(10, 0);
  }

  Serial.println(touch);
}

void setAllPanels() {
  panelSet(panelLeft);
  panelSet(panelCenter);
  panelSet(panelRight);
}

void panelSet(int *panel) {
  if (!touch) { // we're turning on
    int colorCounter = 0;
    int threshold = 127;
    while (colorCounter <= threshold) {
      for (int i = 0; i < 8; i++) {
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
        panelStrip.show();
        colorCounter += 3;
        delay(10);
      }
    }
  } else {
    int colorCounter = 127;
    int threshold = 0;
    while (colorCounter >= threshold) {
      for (int i = 0; i < 8; i++) {
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
        panelStrip.show();
        colorCounter -= 1;
        delay(10);
      }
    }
  }
}

void checkCapSensors() {
  long cap1 =  cs_1.capacitiveSensor(30);
  long cap2 =  cs_2.capacitiveSensor(30);
  if ( cap1 > 200 && cap2 > 200) {
    if (!touch) {
      setAllPanels();
      touch = true;
    }
  } else {
    if (touch) {
      if (offCounter > 100) {
        setAllPanels();
        offCounter = 0;
        touch = false;
      } else {
        offCounter++;
      }
    }
  }
  delay(10);
}

void dockChange() {
  digitalWrite(2, !digitalRead(11));
}


