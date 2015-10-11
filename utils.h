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

void drawMeter(int step){
  // draws a simple bar meter across the front for levels 0-15
  // It's for brightness, but it may be useful for other cases
  if(step < 0 || step > 15){ // Gots ta check them bounds, dig?
    return;
  }

  fillAll(CRGB::DarkBlue);
  for(int i=0; i<16; i++){ // draw a white line across the top.
    leds[XY(i,0)] = CRGB::White;
  }
  for(int i=0; i<=step; i++){
    leds[XY(i,1)] = CRGB::DarkRed; // draw the red bar below it
  }
  FastLED.show();
  effectInit = false;
  effectDelay = 500;
  effectMillis = currentMillis; //reset the effects timer
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

// Determine flash address of text string
unsigned int currentStringAddress = 0;
void selectFlashString(byte string) {
  currentStringAddress = pgm_read_word(&stringArray[string]);
}

// Fetch font character bitmap from flash
byte charBuffer[5] = {0};
void loadCharBuffer(byte character) {
  byte mappedCharacter = character;
  if (mappedCharacter >= 32 && mappedCharacter <= 95) {
    mappedCharacter -= 32; // subtract font array offset
  } else if (mappedCharacter >= 97 && mappedCharacter <= 122) {
    mappedCharacter -= 64; // subtract font array offset and convert lowercase to uppercase
  } else {
    mappedCharacter = 96; // unknown character block
  }
  
  for (byte i = 0; i < 5; i++) {
    charBuffer[i] = pgm_read_byte(Font[mappedCharacter]+i);
  }
  
}

// Fetch a character value from a text string in flash
char loadStringChar(byte string, byte character) {
  return (char) pgm_read_byte(currentStringAddress + character);
}
