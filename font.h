// Fetch font character bitmap from flash
byte charBuffer[5] = {0};

int characterMapping(byte character){
  byte mappedCharacter = character;
  if (mappedCharacter >= 32 && mappedCharacter <= 95) {
    mappedCharacter -= 32; // subtract font array offset
  } else if (mappedCharacter >= 97 && mappedCharacter <= 122) {
    mappedCharacter -= 64; // subtract font array offset and convert lowercase to uppercase
  } else {
    mappedCharacter = 96; // unknown character block
  }
  return mappedCharacter;
}

// variable width pixel font (no lowercase)
// character arrays are 6 bytes, 
// byte 0 is the width of the character
// bytes 1-5 is the bitmask for the pixel columns
const char Font[][6] PROGMEM = {
{0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // 32 <space>
{0b00000001, 0b00010111, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // 33 !
{0b00000011, 0b00000011, 0b00000000, 0b00000011, 0b00000000, 0b00000000}, // 34 "
{0b00000101, 0b00001010, 0b00011111, 0b00001010, 0b00011111, 0b00001010}, // 35 #
{0b00000101, 0b00010010, 0b00010101, 0b00011111, 0b00010101, 0b00001001}, // 36 $
{0b00000101, 0b00010001, 0b00001000, 0b00000100, 0b00000010, 0b00010001}, // 37 %
{0b00000101, 0b00001010, 0b00010101, 0b00001010, 0b00010000, 0b00000000}, // 38 &
{0b00000001, 0b00000011, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // 39 '
{0b00000101, 0b00000000, 0b00000000, 0b00001110, 0b00010001, 0b00000000}, // 40 (
{0b00000101, 0b00000000, 0b00010001, 0b00001110, 0b00000000, 0b00000000}, // 41 )
{0b00000101, 0b00010001, 0b00001010, 0b00011111, 0b00001010, 0b00010001}, // 42 *
{0b00000101, 0b00000100, 0b00000100, 0b00011111, 0b00000100, 0b00000100}, // 43 +
{0b00000010, 0b00010000, 0b00001000, 0b00000000, 0b00000000, 0b00000000}, // 44 ,
{0b00000101, 0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000100}, // 45 -
{0b00000010, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000}, // 46 .
{0b00000101, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001}, // 47 /
{0b00000101, 0b00001110, 0b00010001, 0b00010101, 0b00010001, 0b00001110}, // 48 0
{0b00000101, 0b00000000, 0b00010010, 0b00011111, 0b00010000, 0b00000000}, // 49 1
{0b00000101, 0b00010010, 0b00011001, 0b00010101, 0b00010101, 0b00010010}, // 50 2
{0b00000101, 0b00010101, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 51 3
{0b00000101, 0b00000111, 0b00000100, 0b00000100, 0b00011111, 0b00000100}, // 52 4
{0b00000101, 0b00010111, 0b00010101, 0b00010101, 0b00010101, 0b00001001}, // 53 5
{0b00000101, 0b00001110, 0b00010101, 0b00010101, 0b00010101, 0b00001000}, // 54 6
{0b00000101, 0b00000001, 0b00000001, 0b00011001, 0b00000101, 0b00000011}, // 55 7
{0b00000101, 0b00001010, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 56 8
{0b00000101, 0b00000010, 0b00010101, 0b00010101, 0b00010101, 0b00001110}, // 57 9
{0b00000001, 0b00001010, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // 58 :
{0b00000010, 0b00010000, 0b00001010, 0b00000000, 0b00000000, 0b00000000}, // 59 ;
{0b00000101, 0b00000000, 0b00000100, 0b00001010, 0b00010001, 0b00000000}, // 60 <
{0b00000101, 0b00001010, 0b00001010, 0b00001010, 0b00001010, 0b00001010}, // 61 =
{0b00000101, 0b00000000, 0b00010001, 0b00001010, 0b00000100, 0b00000000}, // 62 >
{0b00000101, 0b00000010, 0b00000001, 0b00010101, 0b00000101, 0b00000010}, // 63 ?
{0b00000101, 0b00011111, 0b00010001, 0b00010101, 0b00010101, 0b00010111}, // 64 @
{0b00000101, 0b00011110, 0b00000101, 0b00000101, 0b00000101, 0b00011110}, // 65 A
{0b00000101, 0b00011111, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 66 B
{0b00000101, 0b00001110, 0b00010001, 0b00010001, 0b00010001, 0b00010001}, // 67 C
{0b00000101, 0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00001110}, // 68 D
{0b00000101, 0b00011111, 0b00010101, 0b00010101, 0b00010001, 0b00010001}, // 69 E
{0b00000101, 0b00011111, 0b00000101, 0b00000101, 0b00000001, 0b00000001}, // 70 F
{0b00000101, 0b00001110, 0b00010001, 0b00010101, 0b00010101, 0b00001101}, // 71 G
{0b00000101, 0b00011111, 0b00000100, 0b00000100, 0b00000100, 0b00011111}, // 72 H
{0b00000101, 0b00010001, 0b00010001, 0b00011111, 0b00010001, 0b00010001}, // 73 I
{0b00000101, 0b00001000, 0b00010000, 0b00010000, 0b00010001, 0b00001111}, // 74 J
{0b00000101, 0b00011111, 0b00000100, 0b00001010, 0b00010001, 0b00010001}, // 75 K
{0b00000101, 0b00011111, 0b00010000, 0b00010000, 0b00010000, 0b00010000}, // 76 L
{0b00000101, 0b00011111, 0b00000010, 0b00000100, 0b00000010, 0b00011111}, // 77 M
{0b00000101, 0b00011111, 0b00000010, 0b00000100, 0b00001000, 0b00011111}, // 78 N
{0b00000101, 0b00001110, 0b00010001, 0b00010001, 0b00010001, 0b00001110}, // 79 O
{0b00000101, 0b00011111, 0b00000101, 0b00000101, 0b00000101, 0b00000010}, // 80 P
{0b00000101, 0b00001110, 0b00010001, 0b00010001, 0b00001001, 0b00010110}, // 81 Q
{0b00000101, 0b00011111, 0b00000101, 0b00000101, 0b00001101, 0b00010010}, // 82 R
{0b00000101, 0b00010010, 0b00010101, 0b00010101, 0b00010101, 0b00001001}, // 83 S
{0b00000101, 0b00000001, 0b00000001, 0b00011111, 0b00000001, 0b00000001}, // 84 T
{0b00000101, 0b00001111, 0b00010000, 0b00010000, 0b00010000, 0b00001111}, // 85 U
{0b00000101, 0b00000011, 0b00001100, 0b00010000, 0b00001100, 0b00000011}, // 86 V
{0b00000101, 0b00000111, 0b00011000, 0b00000110, 0b00011000, 0b00000111}, // 87 W
{0b00000101, 0b00010001, 0b00001010, 0b00000100, 0b00001010, 0b00010001}, // 88 X
{0b00000101, 0b00000001, 0b00000010, 0b00011100, 0b00000010, 0b00000001}, // 89 Y
{0b00000101, 0b00010001, 0b00011001, 0b00010101, 0b00010011, 0b00010001}, // 90 Z
{0b00000101, 0b00000000, 0b00000000, 0b00011111, 0b00010001, 0b00010001}, // 91 [
{0b00000101, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000}, // 92 (\) 
{0b00000101, 0b00010001, 0b00010001, 0b00011111, 0b00000000, 0b00000000}, // 93 ]
{0b00000101, 0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100}, // 94 ^
{0b00000101, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000}, // 95 _
{0b00000101, 0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00011111}}; // 96 <block>