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
#include "EasingLibrary.h"      /* http://andybrown.me.uk/2010/12/05/animation-on-the-arduino-with-easing-functions/ */

/* LED PANEL (https://www.adafruit.com/products/2945) */
const int panelPin = 6;
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, panelPin, NEO_GRB + NEO_KHZ800);
int panelLeft[]   = {0, 1, 8, 9, 16, 17, 24, 25}; // three areas on the board, each 2x4 pixels
int panelCenter[] = {3, 4, 11, 12, 19, 20, 27, 28};
int panelRight[]  = {6, 7, 14, 15, 22, 23, 30, 31};

/* Capacitive touch activates LED panel and vibration motors */
CapacitiveSensor   cs_1 = CapacitiveSensor(5, 12); // 1M resistor between pins
CapacitiveSensor   cs_2 = CapacitiveSensor(9, 13); // 1M resistor between pins
const int touchThreshold = 200;
boolean touch;
int offCounter;

/* Inductive charger as switch - using coil from hacked Clarisonic device */
const int coilPin = A5;
int coilVal;
boolean docked;

/* Vibration motors (https://www.adafruit.com/products/1201) */
const int motorPin = 10;
const int motorSpeed = 255;

/* NeoPixel RGBW ring (https://www.adafruit.com/products/2854) Note! different 3rd parameter in contstructor for this type of LED*/
const int ringPin = 3;
const int ringTotalLEDs = 16;
Adafruit_NeoPixel ring = Adafruit_NeoPixel(ringTotalLEDs, ringPin, NEO_RGBW + NEO_KHZ800);
BackEase ease;       // for "sleep" pulsing effect
double easedPosition, t = 1;
int fadeDirection = 1;
int startLen  = 9;   // for start-up animation, how long is the snake
int startHead = 3;   // for start-up animation, which pixel is the head
int startDelay = 10;
int startLapCounter = 0;
boolean settled = true; // used for escaping the sleep animation if interrupted

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
  
  ease.setDuration(2);
  ease.setTotalChangeInPosition(255);

  enableInterrupt(coilPin, dockChange, CHANGE);
}

///////////////////////////////////////////////////// LOOP /////////////

void loop() {
  checkCapSensors(); // has 10ms delay
  checkDock();       // for stability, this is checked every loop. doesn't cost much. 

  if (docked) {
    settled = true;  // used for escaping the sleep animation if interrupted
    chargingRing();
  } else {
    settled = false;
  }

  if (touch && !docked) { // start active sequence (LED panel + motors). Panels handled in panelSet() w/ touch bool
    analogWrite(motorPin, motorSpeed); 
  } else {
    analogWrite(motorPin, 0); // also appears in panelSet() for timing purposes
  }
}

///////////////////////////////////////////////////// CHARGING RING /////

void chargingRing() {
  for (int i = 0; i <= 255; i++) {
    easedPosition = ease.easeIn(t); // calculates easing value with ease-in & ease-out interpolation
    for (int i = 0; i < ringTotalLEDs; i++) {
      ring.setPixelColor(i, ring.Color(0, 0, t));
      if (settled) {
        ring.show();
      } else { // escape if not docked (otherwise it holds up everything)
        return;
      }
    }
    if (t >= 255 || t <= 0) fadeDirection *= -1;
    t += fadeDirection;
  }
}

///////////////////////////////////////////////////// START RING ////////

void startRing() {
  settled = false; // interrupts chargingRing sleep animation
  while (startLapCounter < 3) {
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
      startLapCounter++;
    } else {
      startHead++;
    }

    ring.show();
    delay(startDelay);
  }
  startLapCounter = 0;
  for (int i=0; i<ringTotalLEDs; i++) {
    ring.setPixelColor(i, ring.Color(0, 0, 0));
  }
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
      for (int i = 0; i < 8; i++) {
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
        panelStrip.show();
        delay(5);
      }
     colorCounter += 9; // to change speed, change the delay, not this counter. it's a multiple of 129 that works well. 
    }
  } else { // we're turning off
    analogWrite(motorPin, 0);
    int colorCounter = 129;
    int threshold = 0;
    while (colorCounter >= threshold) {
      Serial.println("new loop");
      for (int i = 0; i < 8; i++) {
        Serial.println(colorCounter);
        panelStrip.setPixelColor(panel[i], panelStrip.Color(colorCounter, colorCounter, colorCounter));
        panelStrip.show();       
        delay(5);
      }
      colorCounter -= 9; // to change speed, change the delay, not this counter. it's a multiple of 129 that works well. 
    }
  }
}

/////////////////////////////////////////////// CHECK CAP SENSORS /////////

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
    }
    docked = false;
  } else if (coilVal > 350) {  // we are docked (charge from induction coils)
    docked = true;
  }
}


