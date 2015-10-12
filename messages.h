// Scrolling messages

const char string0[] PROGMEM = "RGBShades are totally amazeballs!!!     ";
const char string1[] PROGMEM = "RAINBOW TEXT!  ";
const char string2[] PROGMEM = "macetech.com  ";
//char string3[] PROGMEM = "  ";


// Setup for serial
char messageBuffer[240] = "Aa Bb Cc Dd Ee Ff Gg Hh Ii Jj Kk Ll Mm Nn Oo Pp Qq Rr Ss Tt Uu Vv Ww Xx Yy Zz {|}~!\"#$%&'()*+.-./\\0123456789:;<=>?";
int messageLen = 0;
// {|}~!\"#$%&'()*+.-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`

const char * const stringArray[] PROGMEM = {
  string0,
  string1,
  string2
  //string3

};
