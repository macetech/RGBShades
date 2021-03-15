#pragma once
#include "Arduino.h"
#include <FastLED.h>

// Cycle time (milliseconds between pattern changes)
#define cycleTime 15000

// Hue time (milliseconds between hue increments)
#define hueTime 30

// Time after changing settings before settings are saved to EEPROM
#define EEPROMDELAY 2000

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 72
#define STARTBRIGHTNESS 102

struct Timers {
  uint32_t current = 0;
  uint32_t effect = 0;
  uint32_t cycle = 0;
  uint32_t hue = 0;
  uint32_t fade = 0;
  uint32_t eeprom = 0;
};

struct SystemState {
  boolean serialActive = false;
  uint8_t numEffects = 0;
  boolean effectInit = false; // indicates if a pattern has been recently switched
  uint16_t effectDelay = 0; // time between automatic effect changes
  int currentEffect = 0; // index to the currently running effect
  boolean autoCycle = true; // flag for automatic effect changes
  boolean eepromOutdated = false; // flag for when EEPROM may need to be updated
  byte currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS
  uint8_t cycleHue = 0;
  CRGBPalette16 currentPalette = RainbowColors_p; // global palette storage
};


void hueCycle(byte incr, struct SystemState *state);
void fillAll(CRGB fillColor);
void fadeAll(byte fadeIncr);
void scrollArray(byte scrollDir);
void selectRandomPalette();
void confirmBlink(struct SystemState *state);
void selectFlashString(byte string);
void loadCharBuffer(byte character);
char loadStringChar(byte txtstring, byte character);
void fillnoise8(void);
void selectRandomNoisePalette(void);
void fillnoise8(void);
void checkEEPROM(struct Timers *timers, struct SystemState *state);
void set_serial_active(bool isActive);
bool get_serial_active(void);
