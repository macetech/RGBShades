Pull Requests - For new effects, please target the "experimental" branch. We'll try to throw everything in experimental, and bubble finished effects into the master branch.

RGBShades
=========

Firmware for RGB Shades (Arduino-compatible sunglasses with 16x5 matrix of WS2812B pixels)

Requires FastLED 2.1 or higher, available here: https://github.com/FastLED/FastLED

RGB Shades described here: https://www.kickstarter.com/projects/macetech/rgb-led-shades

When downloading ZIP file, remove "-master" at the end of the folder name before attempting
to open in the Arduino IDE

If your RGB Shades were purchased before July 2015:
This version has the standard Arduino bootloader. R9 and R10 near the control buttons will be present.
Select the “Arduino Pro or Pro Mini” option. Then, go back into the Tools menu and find the Processor option and select “ATmega328 (5V, 16MHz)”.

If your RGB Shades were purchased after July 2015:
This version has the Optiboot bootloader. R9 and 10 near the control buttons will be missing.
Select the “Arduino Mini” option. Then, go back into the Tools menu and find the Processor option and select “ATmega328”.
