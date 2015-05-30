//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
byte sineOffset = 0; // counter for current position of sine waves
void threeSine() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
	if(doHueSubsection)//Pause the effect to show the last saved hue amount
	{
		effectDelay = 600;
		return;
	}
  }
  effectDelay = 20;

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*9+x*16)),2);
      byte sinDistanceG = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*10+x*16)),2);
      byte sinDistanceB = qmul8(abs(y*(255/kMatrixHeight) - sin8(sineOffset*11+x*16)),2);
      
      if(doHueSubsection)
      {
        CRGB firstColor  = SectionCHSV(0, 255, 255);
        CRGB secondColor = SectionCHSV(85, 255, 255);
        CRGB thirdColor  = SectionCHSV(170, 255, 255);
        leds[XY(x,y)] = 
          firstColor.nscale8(255-sinDistanceR) +
          secondColor.nscale8(255-sinDistanceG) +
          thirdColor.nscale8(255-sinDistanceB);
      }else
      {
        leds[XY(x,y)] = CRGB(255-sinDistanceR, 255-sinDistanceG, 255-sinDistanceB);
      } 
    }
  }
  
  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}





// RGB Plasma
byte offset  = 0; // counter for radial color wave motion
int plasVector = 0; // counter for orbiting plasma center
void plasma() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector/256);
  int yOffset = sin8(plasVector/256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x-7.5)*10+xOffset-127)+sq(((float)y-2)*10+yOffset-127))+offset);
      leds[XY(x,y)] = SectionCHSV(color, 255, 255);
    }    
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
byte riderPos = 0;
void rider() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x*(256/kMatrixWidth) - triwave8(riderPos)*2 + 127)*3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = SectionCHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x,y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic

}



// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y <kMatrixHeight; y++) {
      leds[XY(x,y)] = SectionCHSV(cycleHue,255,random8(5)*63);
    }
  }

}


// Fills saturated colors into the array from alternating directions
byte currentColor = 0;
byte currentRow = 0;
byte currentDirection = 0;
CRGB selectedColor = 0;
void colorFill() {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
  }
  if (!doHueSubsection)
	  selectedColor = currentPalette[currentColor];
  else
	  selectedColor = SectionCHSV(currentColor * 16, 255, 255);

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x,y)] = selectedColor;
    }
  } 

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x,y)] = selectedColor;
    }
  }

  currentRow++;
  
  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3,6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }


}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
  }
  
   for (byte x = 0; x < kMatrixWidth; x++) {
     for (byte y = 0; y < kMatrixHeight; y++) {
       if (x < 7) {
         leds[XY(x,y)] = SectionCHSV(170, 255, 255);
       } else if (x > 8) {
         leds[XY(x,y)] = SectionCHSV(0, 255, 255);
       } else {
         leds[XY(x,y)] = CRGB::Black;
       }
     }
   }
   
   leds[XY(6,0)] = CRGB::Black;
   leds[XY(9,0)] = CRGB::Black;

}

// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }
  
  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth-1) * rainDir,y)] = CRGB::Black;
  leds[XY((kMatrixWidth-1)*rainDir, randPixel)] = SectionCHSV(cycleHue, 255, 255); 

}

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
	if (!doHueSubsection)
      leds[XY(random16(kMatrixWidth),random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255);
    else
	  leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = SectionCHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }
   
}


// Draw slanting bars scrolling across the array, uses current hue
byte slantPos = 0;
void slantBars() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x,y)] = SectionCHSV(cycleHue, 255, quadwave8(x*32+y*32+slantPos));
    }
  }

  slantPos-=4;

}
//leds run around the periphery of the shades, changing color every go 'round
boolean erase = false;
uint8_t x,y = 0;
void shadesOutline(){
  //startup tasks
    if (effectInit == false) {
    effectInit = true;
    erase = false;
    uint8_t x=0;
    effectDelay = 15; 
    FastLED.clear();
    currentPalette = RainbowColors_p;}
  const uint8_t OutlineTable[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 43,
     44, 67, 66, 65, 64, 63, 50, 37, 21, 22, 36, 51, 62, 61, 60, 59,
     58, 57, 30, 29};
  if (!doHueSubsection)
    leds[OutlineTable[x]] = currentPalette[currentColor];
  else
	leds[OutlineTable[x]] = SectionCHSV(currentColor * 16, 255, 255);
  if (erase)
    leds[OutlineTable[x]] = CRGB::Black;
   x++; 
  if (x == (sizeof(OutlineTable))) {
    erase = !erase;
    x = 0;
   currentColor += random8(3,6);
   if (currentColor > 15) currentColor -= 16;  
  } 
}
//hearts that start small on the bottom and get larger as they grow upward
const uint8_t SmHeart[] = {46, 48, 53, 55, 60, 65};
const uint8_t MedHeart[] = {31, 32, 34, 35, 38, 39, 
   41, 42, 46, 47, 48, 55, 54, 53, 54, 55, 60, 65};
const uint8_t LrgHeart[] = {15, 16, 18, 19, 24, 25, 
   27, 28, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42, 
   46, 47, 48, 53, 54, 55, 60, 65};
const uint8_t HugeHeart[] = {0, 1, 3, 4, 9, 10, 12, 
   13, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26, 
   27, 28, 29, 31, 32, 33, 34, 35, 38, 39, 40, 41, 
   42, 46, 47, 48, 53, 54, 55, 60, 65};
void hearts() {
   if (effectInit == false) {
    effectInit = true;
    effectDelay = 250; 
    FastLED.clear();
   int x = 0;
   int y = 0;
    }
    effectDelay = 250;
    if (y==5)
     y = 0;
    if (y == 0)
     for (x = 0; x < 6; x++)
      leds[SmHeart[x]] = SectionCHSV(0, 255, 255);
    if (y == 1)
     for (x = 0; x < 18; x++) 
      leds[MedHeart[x]] = SectionCHSV(42, 255, 255);
    if (y == 2)
     for (x = 0; x < 26; x++)
      leds[LrgHeart[x]] = SectionCHSV(84, 255, 255);
    if (y == 3){
     for (x = 0; x < 40; x++)
      leds[HugeHeart[x]] = SectionCHSV(126, 255, 255);
      effectDelay = 450;} //set the delay slightly longer for HUGE heart.
    if (y == 4)
     FastLED.clear();
  y++;
}

//Blasts a hue over all LEDs, the color when the effect ends becomes the start hue for all other effects
void pickHueStartColor() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
    doHueSubsection = true;
  }
  fillAll(SectionCHSV(0, 255, 255));
  incrimentHueOffset(1);
}

//Left to right rainbow, when the effect is over, the length of the hue shift is used to limit hue in all other effects
void pickHueAmount()
{
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    //Add a delay here for a moment to verify settings
    effectDelay = 300;
    return;
  }
  effectDelay = 40;//Speed back to normal

  incrimentHueAmmount(1);
  byte maxLeds = kMatrixHeight * kMatrixWidth;
  //Loop left to right over each led
  for (byte y = 0; y < kMatrixHeight; y++) {
    for (byte x = 0; x < (kMatrixWidth - 0); x++) {
      byte number = (x*kMatrixHeight) + y;
      //Draw a led of the hue of the max value if the division of hue had stopped there
      if ((hueAmmount) >(number * (255 / maxLeds)))
      {
        leds[XY(x, y)] = CHSV(((255. * number) / maxLeds) + hueOffset, 255, 255);
      }
      else
      {
        leds[XY(x, y)] = SectionCHSV(0, 0, 0);
      }
    }
  }
}

