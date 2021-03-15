
#include "utils.h"
#include <FastLED.h>
#include "XYmap.h"
#include "Arduino.h"
#include <EEPROM.h>

extern boolean autoCycle;

// Assorted useful functions and variables



// Increment the global hue value for functions that use it
void hueCycle(byte incr, struct SystemState *state ) {
    state->cycleHue+=incr;
}

// Set every LED in the array to a specified color
void fillAll(CRGB fillColor) {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = fillColor;
  }
}

// Fade every LED in the array by a specified amount
void fadeAll(byte fadeIncr) {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = leds[i].fadeToBlackBy(fadeIncr);
  }
}

// Shift all pixels by one, right or left (0 or 1)
void scrollArray(byte scrollDir) {
  
    byte scrollX = 0;
    for (byte x = 1; x < kMatrixWidth; x++) {
      if (scrollDir == 0) {
        scrollX = kMatrixWidth - x;
      } else if (scrollDir == 1) {
        scrollX = x - 1;
      }
      
      for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XY(scrollX,y)] = leds[XY(scrollX + scrollDir*2 - 1,y)];
      }
    }
  
}


// Pick a random palette from a list
void selectRandomPalette(struct SystemState *state) {

  switch(random8(8)) {
    case 0:
    state->currentPalette = CloudColors_p;
    break;
    
    case 1:
    state->currentPalette = LavaColors_p;
    break;
    
    case 2:
    state->currentPalette = OceanColors_p;
    break;
    
    case 4:
    state->currentPalette = ForestColors_p;
    break;
    
    case 5:
    state->currentPalette = RainbowColors_p;
    break;
    
    case 6:
    state->currentPalette = PartyColors_p;
    break;
    
    case 7:
    state->currentPalette = HeatColors_p;
    break;
  }

}

void selectRandomNoisePalette(struct SystemState *state) {

  switch(random8(8)) {
    case 0:
    state->currentPalette = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Blue);
    break;
    
    case 1:
    state->currentPalette = CRGBPalette16(CRGB::DarkGreen, CRGB::Black, CRGB::Green);
    break;
    
    case 2:
    state->currentPalette = CRGBPalette16(CRGB(0,0,8), CRGB(0,0,16), CRGB(0,0,32), CRGB::White);
    break;
    
    case 3:
    state->currentPalette = CRGBPalette16(CRGB(255,0,127), CRGB::Black, CRGB::OrangeRed);
    break;
    
    case 5:
    state->currentPalette = RainbowColors_p;
    break;
    
    case 6:
    state->currentPalette = PartyColors_p;
    break;
    
    case 7:
    state->currentPalette = HeatColors_p;
    break;
  }

}

// Interrupt normal operation to indicate that auto cycle mode has changed
void confirmBlink(struct SystemState *state) {

  if (state->autoCycle) { // one blue blink, auto mode active
    fillAll(CRGB::DarkBlue);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  } else { // two red blinks, manual mode active
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  }

}

#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
uint8_t noise[16][16];
uint16_t scale = 72;
uint16_t nx;
uint16_t ny;
uint16_t nz;
uint16_t nspeed = 0;

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8(void) {
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      noise[i][j] = inoise8(nx + ioffset,ny + joffset,nz);
    }
  }
  nz += nspeed;
}

byte nextBrightness(boolean resetVal, struct SystemState * state) {
    const byte brightVals[6] = {32,64,96,160,224,255};

    if (resetVal) {
      state->currentBrightness = STARTBRIGHTNESS;
    } else {
      state->currentBrightness++;
      if (state->currentBrightness > sizeof(brightVals)/sizeof(brightVals[0])) state->currentBrightness = 0;
    }

  return brightVals[state->currentBrightness];
}

// Write settings to EEPROM if necessary
void checkEEPROM(struct Timers *timers, struct SystemState *state) {
  if (state->eepromOutdated) {
    if (timers->current - timers->eeprom > EEPROMDELAY) {
      EEPROM.update(0, 99);
      EEPROM.update(1, state->currentEffect);
      EEPROM.update(2, state->autoCycle);
      EEPROM.update(3, state->currentBrightness);
      state->eepromOutdated = false;
    }
  }
}

void set_serial_active(bool isActive);
