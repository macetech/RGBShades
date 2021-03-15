#include "XYmap.h"

CRGB leds[ NUM_LEDS ];

uint16_t XY (uint8_t x, uint8_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = {
    68,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  69,
    29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,
    30,  31,  32,  33,  34,  35,  36,  70,  71,  37,  38,  39,  40,  41,  42,  43,
    57,  56,  55,  54,  53,  52,  51,  73,  72,  50,  49,  48,  47,  46,  45,  44,
    74,  58,  59,  60,  61,  62,  75,  76,  77,  78,  63,  64,  65,  66,  67,  79
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint16_t j = XYTable[i];
  return j;
}


// Map LEDs to shades outline
const uint8_t OutlineTable[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 43,
    44, 67, 66, 65, 64, 63, 50, 37, 21, 22, 36, 51, 62, 61, 60, 59,
    58, 57, 30, 29
};

const uint8_t OUTLINESIZE = sizeof(OutlineTable);

uint8_t OutlineMap(uint8_t i) {
  uint8_t j = OutlineTable[i % OUTLINESIZE];
  return j;
}
