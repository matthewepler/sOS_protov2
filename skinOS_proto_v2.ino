#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <EnableInterrupt.h>
#include "EasingLibrary.h"

const int panelPin = 6;
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, panelPin, NEO_GRB + NEO_KHZ800);
int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25};
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};

CapacitiveSensor   cs_1 = CapacitiveSensor(5, 12); // 1M resistor between pins
CapacitiveSensor   cs_2 = CapacitiveSensor(9, 13); // 1M resistor between pins
const int touchThreshold = 200;
boolean touch;
int offCounter;

const int coilPin = 11;
int changeCounter;
boolean docked;

const int motorPin = 10;
const int motorSpeed = 122;

const int ringPin = 3;
const int ringTotalLEDs = 16;
Adafruit_NeoPixel ring = Adafruit_NeoPixel(ringTotalLEDs, ringPin, NEO_RGBW + NEO_KHZ800);
BackEase ease;
double easedPosition, t = 1;
int fadeDirection = 1;
int startLen  = 3;  // how long is the snake
int startHead = 3; // which pixel is the head
int startDelay = 10;
int startLapCounter = 3;
boolean settled = true;
boolean start = false;

const int debugLED = 2;
long prevDockTime = 0;


void setup() {
  Serial.begin(9600);
  pinMode(debugLED, OUTPUT); // debugging LED
  pinMode(motorPin, OUTPUT);
  pinMode(coilPin, INPUT_PULLUP);

  enableInterrupt(coilPin, dockChange, CHANGE);

  panelStrip.setBrightness(2);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  ring.setBrightness(200);
  ring.begin();
  ease.setDuration(2);
  ease.setTotalChangeInPosition(255);

  dockChange();
}

void loop() {
  checkCapSensors(); // has 10ms delay

  if (start) {
    startRing();
    start = false;
  }

  if (docked) {
    settled = true;
    chargingRing();
  }

  if (touch && !docked) {
    analogWrite(motorPin, motorSpeed); // vibration motors
  } else {
    analogWrite(motorPin, 0);
  }
}

void chargingRing() {
  for (int i = 0; i <= 255; i++) {
    easedPosition = ease.easeIn(t);
    for (int i = 0; i < ringTotalLEDs; i++) {
      ring.setPixelColor(i, ring.Color(0, 0, t));
      if (settled) {
        ring.show();
      } else {
        return;
      }
    }
    if (t >= 255 || t <= 0) fadeDirection *= -1;
    t += fadeDirection;
  }
}

void startRing() {
  for (int lap = 0; lap < startLapCounter; lap++) {
    for (int i = 0; i < ringTotalLEDs; i++) {
      ring.setPixelColor(i, ring.Color(0, 0, 0));
    }

    int pos = startHead;
    for (int j = 0; j < startLen; j++) {
      if (pos >= ringTotalLEDs) {
        pos = 0;
      }
      ring.setPixelColor(pos, ring.Color(0, 0, 255));
      pos++;
    }

    if (startHead >= ringTotalLEDs - 1) {
      startHead = 0;
    } else {
      startHead++;
    }

    ring.show();
    delay(startDelay);
  }
  for (int i=0; i<ringTotalLEDs; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0));
  }
  //enableInterrupt(coilPin, dockChange, CHANGE);
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
  } else { // we're turning off
    analogWrite(motorPin, 0);
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
  if ( cap1 > touchThreshold && cap2 > touchThreshold && !docked) {
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
  Serial.println("CHANGE: " + millis());
  //startRing();
  //disableInterrupt(coilPin);
}

