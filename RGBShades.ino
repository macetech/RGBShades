//   RGB Shades Demo Code
//   Copyright (c) 2014 macetech LLC
//   This software is provided under the MIT License (see license.txt)
//   Special credit to Mark Kriegsman for XY mapping code
//
//   Use Version 3.0 or later https://github.com/FastLED/FastLED
//   ZIP file https://github.com/FastLED/FastLED/archive/master.zip
//
//   Use Arduino IDE 1.0 or later
//   Select device "Arduino Pro or Pro Mini (5V, 16MHz) w/ATmega328
//
//   [Press] the SW1 button to cycle through available effects
//   Effects will also automatically cycle at startup
//   [Press and hold] the SW1 button (one second) to switch between auto and manual mode
//     * Auto Mode (one blue blink): Effects automatically cycle over time
//     * Manual Mode (two red blinks): Effects must be selected manually with SW1 button
//
//   [Press] the SW2 button to cycle through available brightness levels
//   [Press and hold] the SW2 button (one second) to reset brightness to startup value


// RGB Shades data output to LEDs is on pin 5
#define LED_PIN  5

// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 72
#define STARTBRIGHTNESS 127
byte currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS

// Include FastLED library and other useful files
#include <FastLED.h>
#include "messages.h"
#include "font.h"
#include "XYmap.h"
#include "utils.h"
#include "effects.h"
#include "buttons.h"

// Runs one time at the start of the program (power up or reset)
void setup() {

  // write FastLED configuration data
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);//.setCorrection(TypicalSMD5050);
  
  // set global brightness value
  FastLED.setBrightness( scale8(STARTBRIGHTNESS, MAXBRIGHTNESS) );

  // configure input buttons
  pinMode(MODEBUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);

}

// list of functions that will be displayed
functionList effectList[] = {threeSine,
                             threeDee,
                             scrollTextOne,
                             plasma,
                             confetti,
                             rider,
                             scrollTextTwo,
                             glitter,
                             slantBars,
                             colorFill,
                             scrollTextZero,
                             sideRain, 
                             shadesOutline,
                             hearts};

// Timing parameters
#define cycleTime 15000
#define hueTime 30

// Runs over and over until power off or reset
void loop()
{
  currentMillis = millis(); // save the current timer value
  updateButtons(); // read, debounce, and process the buttons
  
  // Check the mode button (for switching between effects)
  switch(buttonStatus(0)) {
    
    case BTNRELEASED: // button was pressed and released quickly
      cycleMillis = currentMillis; 
      if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
    break;
    
    case BTNLONGPRESS: // button was held down for a while
      autoCycle = !autoCycle; // toggle auto cycle mode
      confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
    break;
  
  }
  
  // Check the brightness adjust button  
  switch(buttonStatus(1)) {
    
    case BTNRELEASED: // button was pressed and released quickly
      currentBrightness += 16; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(currentBrightness,MAXBRIGHTNESS));
      drawMeter(currentBrightness/16);
    break;
    
    case BTNLONGPRESS: // button was held down for a while
      currentBrightness = STARTBRIGHTNESS; // reset brightness to startup value
      FastLED.setBrightness(scale8(currentBrightness,MAXBRIGHTNESS));
      drawMeter(currentBrightness/16);
    break;
  
  }
  
  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis; 
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }
  
  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }
  
  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    effectList[currentEffect](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }
  
  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);
      
  FastLED.show(); // send the contents of the led memory to the LEDs

}





