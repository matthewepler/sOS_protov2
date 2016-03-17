#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <EnableInterrupt.h>

const int ledPin = 6;
Adafruit_NeoPixel panelStrip = Adafruit_NeoPixel(32, ledPin, NEO_GRB + NEO_KHZ800);
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

const int debugLED = 2;


void setup() {
  Serial.begin(9600);
  pinMode(debugLED, OUTPUT); // debugging LED
  pinMode(motorPin, OUTPUT);
  pinMode(coilPin, INPUT_PULLUP);

  enableInterrupt(coilPin, dockChange, CHANGE);

  panelStrip.setBrightness(2);
  panelStrip.begin();
  panelStrip.show(); // Initialize all pixels to 'off'

  dockChange();
}

void loop() {
  checkCapSensors(); // has 10ms delay

  if (docked) {
    digitalWrite(debugLED, HIGH);
  } else {
    digitalWrite(debugLED, LOW);
  }

  if (touch && !docked) {
    analogWrite(motorPin, motorSpeed); // vibration motors
  } else {
    analogWrite(motorPin, 0);
  }

  Serial.println(docked);
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
  digitalRead(coilPin)==HIGH ? docked=true : docked=false;
}


