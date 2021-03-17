//   RGB Shades Demo Code
//   Copyright (c) 2021 macetech LLC
//   This software is provided under the MIT License (see license.txt)
//   Special credit to Mark Kriegsman for XY mapping code
//
//   Use Version 3.0 or later https://github.com/FastLED/FastLED
//   ZIP file https://github.com/FastLED/FastLED/archive/master.zip
//
//   [Press] the SW1 button to cycle through available effects
//   [Press and hold] the SW1 button (one second) to switch between auto and manual mode
//     * Auto Mode (one blue blink): Effects automatically cycle over time
//     * Manual Mode (two red blinks): Effects must be selected manually with SW1 button
//
//   [Press] the SW2 button to cycle through available brightness levels
//   [Press and hold] the SW2 button (one second) to reset brightness to startup value
//
//   Brightness, selected effect, and auto-cycle are saved in EEPROM after a delay
//   The RGB Shades will automatically start up with the last-selected settings


// RGB Shades data output to LEDs is on pin 5
#define LED_PIN  5

// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#include <FastLED.h>
#include <EEPROM.h>

#include "XYmap.h"
#include "utils.h"
#include "effects.h"
#include "buttons.h"
#include "messages.h"
#include "serialcomms.h"

Message text1;

Effect * Effects[] = {
  &text1,
  new Plasma,
  new Rider,
  new ThreeSine,
  &text1,
  new Glitter,
  new ColorFill,
  new ThreeDee,
  new SideRain,
  &text1,
  new Confetti,
  new SlantBars,
  new ShadesOutline,
  //new NoiseFlyer
};

struct SystemState systemstate;
struct SystemState * Effect::state = &systemstate;
struct Timers timers;

void setup() {

  // Initialize number of loaded effects
  systemstate.numEffects = (sizeof(Effects)/sizeof(Effects[0]));

  Serial.begin(9600);

  // check to see if EEPROM has been used yet
  // if so, load the stored settings
  byte eepromWasWritten = EEPROM.read(0);
  if (eepromWasWritten == 99) {
    systemstate.currentEffect = EEPROM.read(1);
    systemstate.autoCycle = EEPROM.read(2);
    systemstate.currentBrightness = EEPROM.read(3);
  }

  if (systemstate.currentEffect > (systemstate.numEffects - 1)) systemstate.currentEffect = 0;

  // write FastLED configuration data
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, LAST_VISIBLE_LED + 1);

  // set global brightness value
  FastLED.setBrightness( scale8(systemstate.currentBrightness, MAXBRIGHTNESS) );
  FastLED.setDither(0);

  // configure input buttons
  pinMode(MODEBUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESSBUTTON, INPUT_PULLUP);

}

void loop()
{
  // Check for Web Serial text editing functions
  process_serial();

  timers.current = millis(); // save the current timer value
  doButtons(&timers, &systemstate);              // perform actions based on button state
  checkEEPROM(&timers, &systemstate);            // update the EEPROM if necessary

  // If serial control detected, only display text scroll
  Effect *effect;
  if (get_serial_active()) {
    systemstate.autoCycle = false;
    effect = &text1;
  } else {
    effect = Effects[systemstate.currentEffect];
  }

  // switch to a new effect every cycleTime milliseconds
  if (timers.current - timers.cycle > cycleTime && systemstate.autoCycle == true) {
    timers.cycle = timers.current;
    if (++systemstate.currentEffect >= systemstate.numEffects) systemstate.currentEffect = 0; // loop to start of effect list
    systemstate.effectInit = false;
  }

  // increment the global hue value every hueTime milliseconds
  if (timers.current - timers.hue > hueTime) {
    timers.hue = timers.current;
    hueCycle(1, &systemstate); // increment the global hue value
  }

  // Control fading effect if active
  if (timers.current - timers.fade > 1) {
        timers.fade = timers.current;
        fadeAll(effect->fade);
  }

  // run the currently selected effect every effectDelay milliseconds
  if (timers.current - timers.effect > effect->delayMillis) {
    timers.effect = timers.current;
    if (systemstate.effectInit == false) effect->init();
    effect->render();    
    random16_add_entropy(1); // make the random values a bit more random-ish
  }


  FastLED.show(); // send the contents of the led memory to the LEDs

}
