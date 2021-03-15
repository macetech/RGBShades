#pragma once
#include "FastLED.h"

// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman
//
// Params for width and height
constexpr uint8_t kMatrixWidth = 16;
constexpr uint8_t kMatrixHeight = 5;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define LAST_VISIBLE_LED 67

extern CRGB leds[];
uint16_t XY (uint8_t x, uint8_t y);
uint8_t OutlineMap(uint8_t i);
extern const uint8_t OUTLINESIZE;
