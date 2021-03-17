#include "buttons.h"
#include "utils.h"

static unsigned long buttonEvents[NUMBUTTONS];
static byte buttonStatuses[NUMBUTTONS];
static byte buttonmap[NUMBUTTONS] = {BRIGHTNESSBUTTON, MODEBUTTON};

static void updateButtons(struct Timers *timers) {
  for (byte i = 0; i < NUMBUTTONS; i++) {
    switch (buttonStatuses[i]) {
      case BTNIDLE:
        if (digitalRead(buttonmap[i]) == LOW) {
          buttonEvents[i] = timers->current;
          buttonStatuses[i] = BTNDEBOUNCING;
        }
        break;

      case BTNDEBOUNCING:
        if (timers->current - buttonEvents[i] > BTNDEBOUNCETIME) {
          if (digitalRead(buttonmap[i]) == LOW) {
            buttonStatuses[i] = BTNPRESSED;
          }
        }
        break;

      case BTNPRESSED:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNRELEASED;
        } else if (timers->current - buttonEvents[i] > BTNLONGPRESSTIME) {
          buttonStatuses[i] = BTNLONGPRESS;
        }
        break;

      case BTNRELEASED:
        break;

      case BTNLONGPRESS:
        break;

      case BTNLONGPRESSREAD:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNIDLE;
        }
        break;
    }
  }
}

static byte buttonStatus(byte buttonNum) {

  byte tempStatus = buttonStatuses[buttonNum];
  if (tempStatus == BTNRELEASED) {
    buttonStatuses[buttonNum] = BTNIDLE;
  } else if (tempStatus == BTNLONGPRESS) {
    buttonStatuses[buttonNum] = BTNLONGPRESSREAD;
  }

  return tempStatus;

}

void doButtons(struct Timers *timers, struct SystemState *state) {

  updateButtons(timers);
  
  // Check the mode button (for switching between effects)
  switch (buttonStatus(0)) {

    case BTNRELEASED: // button was pressed and released quickly
      timers->cycle = timers->current;
      if (++state->currentEffect >= state->numEffects) state->currentEffect = 0; // loop to start of effect list
      state->effectInit = false; // trigger effect initialization when new effect is selected
      timers->eeprom = timers->current;
      state->eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      state->autoCycle = !state->autoCycle; // toggle auto cycle mode
      confirmBlink(state); // one blue blink: auto mode. two red blinks: manual mode.
      timers->eeprom = timers->current;
      state->eepromOutdated = true;
      break;

  }

  // Check the brightness adjust button
  switch (buttonStatus(1)) {

    case BTNRELEASED: // button was pressed and released quickly
      state->currentBrightness += 51; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(state->currentBrightness, MAXBRIGHTNESS));
      timers->eeprom = timers->current;
      state->eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      state->currentBrightness = STARTBRIGHTNESS; // reset brightness to startup value
      FastLED.setBrightness(scale8(state->currentBrightness, MAXBRIGHTNESS));
      timers->eeprom = timers->current;
      state->eepromOutdated = true;
      break;

  }
  
}
