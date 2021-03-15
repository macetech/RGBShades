#pragma once
#include "utils.h"
#include "XYmap.h"

// TODO: Update this
//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

class Effect {
  protected:
    Effect() {};
  public:
    bool initialized;
    int delayMillis;
    int fade;
    bool done;
    static SystemState *state;
    virtual void init() =0;
    virtual void render() =0;
};

class Plasma : public Effect {
  private:
    byte offset = 0; // counter for radial color wave motion
    int plasVector = 0; // counter for orbiting plasma center
  public:
    Plasma() { init(); }
    void init(void) {
        delayMillis = 10;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      // Calculate current center of plasma pattern (can be offscreen)
      int xOffset = cos8(plasVector / 256);
      int yOffset = sin8(plasVector / 256);
    
      // Draw one frame of the animation into the LED array
      for (int x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
          byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
          leds[XY(x, y)] = CHSV(color, 255, 255);
        }
      }
      offset++; // wraps at 255 for sin8
      plasVector += 16; // using an int for slower orbit (wraps at 65536)
    }
};

class Rider : public Effect {
  private:
    byte riderPos = 0;
  public:
    Rider() { init(); }

    void init(void) {
        riderPos = 0;
        delayMillis = 5;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      // Draw one frame of the animation into the LED array
      for (byte x = 0; x < kMatrixWidth; x++) {
        int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
        if (brightness > 255) brightness = 255;
        brightness = 255 - brightness;
        CRGB riderColor = CHSV(state->cycleHue, 255, brightness);
        for (byte y = 0; y < kMatrixHeight; y++) {
          leds[XY(x, y)] = riderColor;
        }
      }
      riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
    }
};

class ThreeSine : public Effect {
  private:
    byte sineOffset = 0; // counter for current position of sine waves
  public:
    ThreeSine() { init(); }
    
    void init(void) {
        delayMillis = 20;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      // Draw one frame of the animation into the LED array
      for (byte x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
          // Calculate sine waves with varying periods
          // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
          byte sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 9 + x * 16)), 2);
          byte sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 10 + x * 16)), 2);
          byte sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 11 + x * 16)), 2);
          leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
        }
      }
      sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle
    }
};

// Shimmering noise, uses global hue cycle
class Glitter : public Effect {
  private:
  public:
    Glitter() { init(); }
    void init(void) {
        delayMillis = 30;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      // Draw one frame of the animation into the LED array
      for (int x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
          leds[XY(x, y)] = CHSV(state->cycleHue, 255, random8(5) * 63);
        }
      }
    }
};

// Fills saturated colors into the array from alternating directions
class ColorFill : public Effect {
  private:
    byte currentColor = 0;
    byte currentRow = 0;
    byte currentDirection = 0;
  public:
    ColorFill() { init(); }

    void init(void) {
        delayMillis = 45;
        fade = 0;
        currentColor = 0;
        currentRow = 0;
        currentDirection = 0;
        state->currentPalette = RainbowColors_p;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      
      // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
      if (!(currentDirection & 1)) {
        delayMillis = 45; // slower since vertical has fewer pixels
        for (byte x = 0; x < kMatrixWidth; x++) {
          byte y = currentRow;
          if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
          leds[XY(x, y)] = state->currentPalette[currentColor];
        }
      }
    
      // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
      if (currentDirection & 1) {
        delayMillis = 20; // faster since horizontal has more pixels
        for (byte y = 0; y < kMatrixHeight; y++) {
          byte x = currentRow;
          if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
          leds[XY(x, y)] = state->currentPalette[currentColor];
        }
      }
      currentRow++;
    
      // detect when a fill is complete, change color and direction
      if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
        currentRow = 0;
        currentColor += random8(3, 6);
        if (currentColor > 15) currentColor -= 16;
        currentDirection++;
        if (currentDirection > 3) currentDirection = 0;
        delayMillis = 300; // wait a little bit longer after completing a fill
      }
    }
};

// Emulate 3D anaglyph glasses
class ThreeDee : public Effect {
  private:
  public:
    ThreeDee() { init(); }
    void init(void) {
        delayMillis = 50;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      for (byte x = 0; x < kMatrixWidth; x++) {
        for (byte y = 0; y < kMatrixHeight; y++) {
          if (x < 7) {
            leds[XY(x, y)] = CRGB::Blue;
          } else if (x > 8) {
            leds[XY(x, y)] = CRGB::Red;
          } else {
            leds[XY(x, y)] = CRGB::Black;
          }
        }
      }
      leds[XY(6, 0)] = CRGB::Black;
      leds[XY(9, 0)] = CRGB::Black;
    }
};

// Random pixels scroll sideways, uses current hue
class SideRain : public Effect {
  private:
  public:
    SideRain() { init(); }
    void init(void) {
        delayMillis = 20;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    #define rainDir 0
    void render() {
      scrollArray(rainDir);
      byte randPixel = random8(kMatrixHeight);
      for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
      leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(state->cycleHue, 255, 255);
    }
};

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
class Confetti : public Effect {
  private:
  public:
    Confetti() { init(); }
    
    void init(void) {
        delayMillis = 10;
        fade = 1;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render(void) {
      // scatter random colored pixels at several random coordinates
      for (byte i = 0; i < 4; i++) {
        leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = ColorFromPalette(state->currentPalette, random16(255), 255);
        random16_add_entropy(1);
      }
    }
};

// Draw slanting bars scrolling across the array, uses current hue
class SlantBars : public Effect {
  private:
    byte slantPos = 0;
  public:
    SlantBars() { init(); }
    void init(void) {
        slantPos = 0;
        delayMillis = 5;
        fade = 0;
        fillAll(0);
        state->effectInit = true;
    }
    
    void render() {
      for (byte x = 0; x < kMatrixWidth; x++) {
        for (byte y = 0; y < kMatrixHeight; y++) {
          leds[XY(x, y)] = CHSV(state->cycleHue, 255, quadwave8(x * 32 + y * 32 + slantPos));
        }
      }
      slantPos -= 4;
    }
};

//leds run around the periphery of the shades, changing color every go 'round
class ShadesOutline : public Effect {
  private:
    uint8_t x = 0;
  public:
    ShadesOutline() { init(); }
    
    void init(void) {
        x = 0;
        delayMillis = 25;
        fade = 1;
        fillAll(0);
        state->currentPalette = RainbowColors_p;
        state->effectInit = true;
    }
    
    void render() {
      CRGB pixelColor = CHSV(state->cycleHue, 255, 255);
      leds[OutlineMap(x)] = pixelColor;
      x++;
      if (x > (OUTLINESIZE - 1)) x = 0;
    }
};


class NoiseFlyer : public Effect {
  private:
    uint8_t x = 0;
  public:
    NoiseFlyer() { init(); }
    void render(void);
    void init(void);
};







































/*



//hearts that start small on the bottom and get larger as they grow upward
const uint8_t SmHeart[] = {46, 48, 53, 55, 60, 65};
const uint8_t MedHeart[] = {31, 32, 34, 35, 38, 39,
                            41, 42, 46, 47, 48, 55, 54, 53, 54, 55, 60, 65
                           };
const uint8_t LrgHeart[] = {15, 16, 18, 19, 24, 25,
                            27, 28, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42,
                            46, 47, 48, 53, 54, 55, 60, 65
                           };
const uint8_t HugeHeart[] = {0, 1, 3, 4, 9, 10, 12,
                             13, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26,
                             27, 28, 29, 31, 32, 33, 34, 35, 38, 39, 40, 41,
                             42, 46, 47, 48, 53, 54, 55, 60, 65
                            };
void hearts() {
  static boolean erase = false;
  static uint8_t x, y = 0;
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 150;
    FastLED.clear();
    x = 0;
    y = 0;
  }
  if (y == 5)
    y = 0;
  if (y == 0)
    for (x = 0; x < 6; x++)
      leds[SmHeart[x]] = CRGB::Salmon; //Tried to transition from pink-ish to red. Kinda worked.
  if (y == 1)
    for (x = 0; x < 18; x++)
      leds[MedHeart[x]] = CRGB::Tomato;
  if (y == 2)
    for (x = 0; x < 26; x++)
      leds[LrgHeart[x]] = CRGB::Crimson;
  if (y == 3) {
    for (x = 0; x < 40; x++)
      leds[HugeHeart[x]] = CRGB::Red;
  } //set the delay slightly longer for HUGE heart.
  if (y == 4)
    FastLED.clear();
  y++;
}

// RGB Plasma
void spinPlasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = (cos8(plasVector)-127)/2;
  int yOffset = (sin8(plasVector)-127)/2;

  //int xOffset = 0;
  //int yOffset = 0;


  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      leds[XY(x, y)] = ColorFromPalette(state->currentPalette, color, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 1; // using an int for slower orbit (wraps at 65536)

}

*/
