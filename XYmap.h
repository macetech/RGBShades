#pragma once
#include "FastLED.h"

// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman
//
// Params for width and height
#define kMatrixWidth (16)
#define kMatrixHeight (5)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define LAST_VISIBLE_LED (67)

extern CRGB leds[];
extern const uint8_t OUTLINESIZE;
uint16_t XY (uint8_t x, uint8_t y);
uint8_t OutlineMap(uint8_t i);
