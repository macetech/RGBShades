#pragma once
#include "Arduino.h"
// Process button inputs and return button activity

#define NUMBUTTONS 2
#define MODEBUTTON 4
#define BRIGHTNESSBUTTON 3

#define BTNIDLE 0
#define BTNDEBOUNCING 1
#define BTNPRESSED 2
#define BTNRELEASED 3
#define BTNLONGPRESS 4
#define BTNLONGPRESSREAD 5

#define BTNDEBOUNCETIME 20
#define BTNLONGPRESSTIME 1000

extern boolean autoCycle;

void updateButtons(struct Timers *timers);
void doButtons(struct Timers *timers, struct SystemState *state);
