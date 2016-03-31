/*
  Matthew Epler
  for Human Condition Labs & SkinOS
  2016
  Do not use without permission

  Board = Adafruit Feather 32u4 Proto board (https://www.adafruit.com/products/2771)
         Note: Feather M0 is not compatible with capcitive sensor library

  IDE = Arduino v 1.6.8
*/

#include <Adafruit_NeoPixel.h>  /* https://github.com/adafruit/Adafruit_NeoPixel */
#include <CapacitiveSensor.h>   /* https://github.com/PaulStoffregen/CapacitiveSensor */
#include <EnableInterrupt.h>    /* https://github.com/GreyGnome/EnableInterrupt */

/* LED PANEL (https://www.adafruit.com/products/2945) */
const int panelPin = 6;
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, panelPin, NEO_GRB + NEO_KHZ800);
int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25}; // three areas on the board, each 2x4 pixels
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};
const int panelSize = 8;

/* Capacitive touch activates LED panel and vibration motors */
CapacitiveSensor   cs_1 = CapacitiveSensor(5, 12); // 1M resistor between pins
CapacitiveSensor   cs_2 = CapacitiveSensor(9, 13); // 1M resistor between pins
const int touchThreshold = 200;
boolean touch;
int offCounter;

/* Inductive charger as switch - using coil from hacked Clarisonic device */
const int coilPin = A5;
const int interruptPin = 2;
volatile int coilVal;
boolean docked;

/* Vibration motors (https://www.adafruit.com/products/1201) */
const int motorPin   = 10;
const int motorSpeed = 255;

/* NeoPixel RGBW ring (https://www.adafruit.com/products/2854) Note! different 3rd parameter in contstructor for this type of LED*/
const int ringPin = 3;
const int ringTotalLEDs = 16;
Adafruit_NeoPixel ring = Adafruit_NeoPixel(ringTotalLEDs, ringPin, NEO_RGBW + NEO_KHZ800);
double blue         = 1;
int blueThresh      = 150;
int fadeDirection   = 1;
int startLen        = 9;   // for start-up animation, how long is the snake
int startHead       = 3;   // for start-up animation, which pixel is the head
int startDelay      = 10;
int startLapCounter = 0;
boolean settled = true; // used for escaping the sleep animation if interrupted

/* Dev debug vars */
const int debugLED = 2; // not used in production


///////////////////////////////////////////////////// SETUP /////////////

void setup() {
  Serial.begin(9600);
  pinMode(debugLED, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(coilPin, INPUT_PULLUP);

  panelStrip.setBrightness(50);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  ring.setBrightness(200);
  ring.begin();
  ring.show();

  enableInterrupt(interruptPin, dockChange, CHANGE);

}

///////////////////////////////////////////////////// LOOP /////////////

void loop() { 
  checkCapSensors(); // has 10ms delay
  checkDock();

  Serial.println(analogRead(coilPin));
  Serial.println(docked);

  settled = docked;
  if (docked) {
    chargingRing();
  }

  if (touch && !docked) { // start active sequence (LED panel + motors). Panels handled in panelSet() w/ touch bool
    analogWrite(motorPin, motorSpeed);
  } else {
    analogWrite(motorPin, 0); // also appears in panelSet() for timing purposes
  }
}

///////////////////////////////////////////////////// CHARGING RING /////

void chargingRing() {
  for (int i = 0; i <= blueThresh; i++) {
    for (int i = 0; i < ringTotalLEDs; i++) {
      ring.setPixelColor(i, ring.Color(0, 0, blue));
      if (settled) {
        ring.show();
      } else { // escape if not docked (otherwise it holds up everything)
        return;
      }
    }
    if (blue >= blueThresh || blue <= 0) {
      fadeDirection = -fadeDirection;
    }
    blue += fadeDirection;
  }
}

///////////////////////////////////////////////////// START RING ////////

void startRing() {
  settled = false; // interrupts chargingRing sleep animation
  while (startLapCounter < 3) {
    clearPixels();

    int pos = startHead;
    for (int j = 0; j < startLen; j++) {
      ring.setPixelColor(pos++, ring.Color(0, 0, 255));
      if (pos > ringTotalLEDs) {
        pos = 0;
      }
    }

    if (startHead > ringTotalLEDs) {
      startHead = 0;
      startLapCounter++;
    } else {
      startHead++;
    }

    ring.show();
    delay(startDelay);
  }
  startLapCounter = 0;
  clearPixels();
  ring.show();
}

//////////////////////////////////////////////// SET ALL PANELS //////////

void setAllPanels() {
  panelSet(panelLeft);
  panelSet(panelCenter);
  panelSet(panelRight);
}

///////////////////////////////////////////////////// PANEL SET //////////

void panelSet(int *panel) {
  if (!touch) { // we're turning on
    int colorCounter = 0;
    int threshold = 129;
    while (colorCounter <= threshold) {
      setLeds(panel, colorCounter);
      colorCounter += 9; // to change speed, change the delay, not this counter. it's a multiple of 129 that works well.
    }
  } else { // we're turning off
    analogWrite(motorPin, 0);
    int colorCounter = 129;
    int threshold = 0;
    while (colorCounter >= threshold) {
      setLeds(panel, colorCounter);
      colorCounter -= 9; // to change speed, change the delay, not this counter. it's a multiple of 129 that works well.
    }
  }
}

///////////////////////////////////////////////////// PANEL SET //////////
void setLeds(int *panel, int colorCounter) {
  for (int i = 0; i < panelSize; i++) { // ***
    panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
    panelStrip.show();
    delay(5);
  }
}

/////////////////////////////////////////////// CHECK CAP SENSORS /////////

void checkCapSensors() {
  if (!docked) {
    long cap1 =  cs_1.capacitiveSensor(30);
    long cap2 =  cs_2.capacitiveSensor(30);

    if (cap1 > touchThreshold && cap2 > touchThreshold) {
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
}

///////////////////////////////////////////////////// DOCK CHANGE //////////

void dockChange() { // kept this separate in case we add to the interrupt handler later, also checkDock is used by loop.
  checkDock();
}

///////////////////////////////////////////////////// CHECK DOCK ///////////

void checkDock() {
  coilVal = analogRead(coilPin);
  if (coilVal < 200) { // if we were docked but now we're not (low charge from induction coils)
    if (docked) {
      startRing();
      docked = false;
    }
  } else if (coilVal > 350) {  // we are docked (charge from induction coils)
    docked = true;
  }
}

///////////////////////////////////////////////////// CLEAR NEOPIXELS //////

void clearPixels() {
  for (int i = 0; i < ringTotalLEDs; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0));
  }
}


