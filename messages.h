// Scrolling messages

const char string0[] PROGMEM = "HELLO WORLD ";
const char string1[] PROGMEM = "RAINBOW TEXT!  ";
const char string2[] PROGMEM = "macetech.com  ";
//char string3[] PROGMEM = "  ";



// Setup for serial
char messageBuffer[140] = " 'apple' 'orange' apple's orange's";
int messageLen = 0;

const char * const stringArray[] PROGMEM = {
  string0,
  string1,
  string2
  //string3
};
