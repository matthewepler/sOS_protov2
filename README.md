# SkinOS Prototype V2 (Arduino)
A look/feel prototype for "Zara" device. 

### Features/Functionality:

 * Breathing LED animation when on docked
 * Detect when undocked (using induction coils)
 * When undocked, startup LED animation, light-up LED panel, vibration motors on
 * When re-docked, return to breathing animation

The dock and the device's matching internal induction coil were taken from a Clarisonic device. 

### Dependecies:
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) 
* [CapacitiveSensor](https://github.com/PaulStoffregen/CapacitiveSensor)
* [EnableInterrupt](https://github.com/GreyGnome/EnableInterrupt )
* [EasingLibrary](http://andybrown.me.uk/2010/12/05/animation-on-the-arduino-with-easing-functions/)


### Hardware
* [Adafruit Feather 32u4 Proto](https://www.adafruit.com/products/2771)
* [NeoPixel Feather Board](https://www.adafruit.com/products/2945)
* [NeoPixel 16 RGBW ring](https://www.adafruit.com/products/2854)
* [5x mini vibration motors](https://www.adafruit.com/products/1201)


