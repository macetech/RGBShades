// Assorted useful functions and variables

boolean effectInit = false; // indicates if a pattern has been recently switched
uint16_t effectDelay = 0; // time between automatic effect changes
unsigned long effectMillis = 0; // store the time of last effect function run
unsigned long cycleMillis = 0; // store the time of last effect change
unsigned long currentMillis; // store current loop's millis value
unsigned long hueMillis; // store time of last hue change

byte currentEffect = 0; // index to the currently running effect
boolean autoCycle = true; // flag for automatic effect changes

CRGBPalette16 currentPalette(RainbowColors_p); // global pallete storage


typedef void (*functionList)(); // definition for list of effect function pointers
#define numEffects (sizeof(effectList) / sizeof(effectList[0]))


// Increment the global hue value for functions that use it
byte cycleHue = 0;
byte cycleHueCount = 0;
void hueCycle(byte incr) {
    cycleHueCount = 0;
    cycleHue+=incr;
}

/* Instead of a perpetually incrimenting hue, override hue calculations to instead do a
 * sinusodial loop back and forth over a smaller section of the scale.
 * This can allow various effects, ice, hue, or even single colors.
 * If the hue ammount is close to max, switch back to an incrimental mode and allow pallets
 * to be used again.  Continually incrimenting the offset and ammount allows the autoCycle
 * mode to keep making new sections.
 */
//Keep making new effects.
byte hueOffset = 0;//The start hue for our cycle
byte hueAmmountIncrimentor = 0;//incrimentor for generating the hueAmmount
byte hueAmmount = 1;//The distance across the hue scale to allow sliding
bool doHueSubsection = false;//Switch to go back to a continuous hue slide (no more back & forth)
CHSV SectionCHSV(uint8_t h, uint8_t s, uint8_t v)
{
	//return a hue from a sinusodial wave across our current section of the hue scale
	if (doHueSubsection)return CHSV((quadwave8(h) / (255. / hueAmmount)) + hueOffset, s, v);
	return CHSV(h, s, v);
}
void incrimentHueOffset(uint8_t offset)
{
	hueOffset += offset;
}
void incrimentHueAmmount(uint8_t offset)
{
	hueAmmountIncrimentor += offset;
	//Don't actually increase it till it loops to 0, instead make it go up, then back down.
	hueAmmount = triwave8(hueAmmountIncrimentor);
	doHueSubsection = hueAmmount < 230;//If the hue amount is almost a full rainbow, make it a full rainbow to allow pallet effects and others
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
void selectRandomPalette() {

  switch(random8(8)) {
    case 0:
    currentPalette = CloudColors_p;
    break;
    
    case 1:
    currentPalette = LavaColors_p;
    break;
    
    case 2:
    currentPalette = OceanColors_p;
    break;
    
    case 4:
    currentPalette = ForestColors_p;
    break;
    
    case 5:
    currentPalette = RainbowColors_p;
    break;
    
    case 6:
    currentPalette = PartyColors_p;
    break;
    
    case 7:
    currentPalette = HeatColors_p;
    break;
  }


}

// Interrupt normal operation to indicate that auto cycle mode has changed
void confirmBlink() {

  if (autoCycle) { // one blue blink, auto mode active
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
