// Scrolling messages
#pragma once
#include "effects.h"
#include "XYmap.h"
#include "font.h"
#include <EEPROM.h>

class Message : public Effect {
  private:
    char bit_buffer[kMatrixWidth]; // Bitmap text column buffer
    uint8_t bit_buffer_idx = 0;

    // Character stream tracking
    char current_char; // Character to display
    const char * current_char_font; // Font for current character
    uint8_t msg_pos = 0;
    uint8_t char_col = 0;

    uint8_t paletteCycle = 0;
    enum styles {
      STYLE_NORMAL = 1,
      STYLE_RAINBOW,
      STYLE_RAINBOWBG,
    };
    
    // Return pointer to bitmap font for a given ASCII character
    const char * get_char_font(char character) {
      if (character >= 32 && character <= 95) {
        character -= 32; // subtract font array offset
      } else if (character >= 97 && character <= 122) {
        character -= 64; // subtract font array offset and convert lowercase to uppercase
      } else {
        character = 96; // unknown character block
      }
  
      return Font[(byte)character];
    }

    // Copy specified column of bitmap font to bitmap buffer and increment buffer index
    void load_char_column(const char * char_font, uint8_t column) {
      if (column < 5) {
        bit_buffer[bit_buffer_idx] = pgm_read_byte(char_font+column); // Get column from font
      } else {
        bit_buffer[bit_buffer_idx] = 0; // Set to zero (spacing between characters)
      }

      bit_buffer_idx++;
      if (bit_buffer_idx >= kMatrixWidth) bit_buffer_idx = 0;
    }

    // Retrieve characters from message string one at a time, manage character columns
    void stream_chars(void) {
      if (char_col == 0) {
        current_char = curr_msg_info.msg[msg_pos++];
        if (current_char == 0) {
          msg_pos = 0; // restart string at the end
          current_char = curr_msg_info.msg[msg_pos++];
          if (curr_msg_info.reps > 0) {
            curr_msg_info.reps -=1;
          } else if (curr_msg_info.reps == 0) {
            done = true;
          } // set msg_reps to -1 for infinite repeat
        } // TODO: inline formatting
        current_char_font = get_char_font(current_char);
        // TODO: reps
      }
      load_char_column(current_char_font, char_col);
      if (++char_col >= (CHAR_WIDTH + CHAR_SPACING)) {
        char_col = 0;
      }
    }

  public:
    static const uint8_t MAX_MESSAGES = 10;
    uint8_t msg_enabled[MAX_MESSAGES];
    uint8_t msg_count;
    static const uint8_t MAX_MESSAGE_LENGTH = 64;
    static const uint16_t EEPROM_MSG_SEQ_START = 16;
    static const uint16_t EEPROM_MSG_START = (EEPROM_MSG_SEQ_START + sizeof msg_enabled);
    static const uint16_t EEPROM_MAX_ADDR = (1023-5); // ATmega328P
    static const char MSG_START_CHAR = 0x00;
    static const char MSG_END_CHAR = 0xFF;
    static const uint8_t CHAR_WIDTH = 5;
    static const uint8_t CHAR_SPACING = 2;
    static const uint8_t MIN_INTERVAL = 15;
 
  
    struct msg_info {
      uint8_t enabled = 0;
      int8_t reps = 0;
      uint8_t run_interval = 35;
      uint8_t style = 0;
      CRGB fgcolor = 0;
      CRGB bgcolor = 0;
      char msg[MAX_MESSAGE_LENGTH+1];
    };
    
    struct msg_info curr_msg_info;
    uint8_t curr_msg;
    uint8_t msg_index;
    uint8_t next_empty_slot;

    void load_run_interval(void) {
      if (curr_msg_info.run_interval > MIN_INTERVAL) delayMillis = curr_msg_info.run_interval;
      else delayMillis = MIN_INTERVAL;
    }

    // Copy message from EEPROM to current message struct
    bool load_message(uint8_t message) {
      curr_msg = (message & ~(1<<7));
      EEPROM.get(EEPROM_MSG_START + curr_msg * sizeof curr_msg_info, curr_msg_info);
      load_run_interval();
      //if (strlen(curr_msg_info.msg) < 1) curr_msg_info.enabled = 0; // Don't try display null characters
      if (msg_count == 0 || message > (msg_count - 1)) return false;
      else return true;
    }

    // Cycle through messages in enabled list
    void next_message(void) {
      msg_index++;
      if (msg_index >= msg_count) msg_index = 0;
      load_message(msg_enabled[msg_index]);
    }

    // Save the currently running message in an EEPROM slot
    void save_message(uint8_t message) {
      EEPROM.put(EEPROM_MSG_START + (message & ~(1<<7)) * sizeof curr_msg_info, curr_msg_info);
    }

    // Write zeroes to selected message area in EEPROM
    void erase_message(uint8_t message) {
      memset(&curr_msg_info, 0, sizeof curr_msg_info);
      curr_msg_info.enabled = 255;  
      save_message(message);
    }

    // Erase the last slot in use
    void erase_last_message(void) {
      if (next_empty_slot == 255) { // All slots in use
        erase_message(MAX_MESSAGES - 1);
      } else if (next_empty_slot > 0) { // One or more slots in use
        erase_message(next_empty_slot - 1);
      }
      update_msg_list();
    }

    // Set up a new message slot at the end of the current list
    bool add_new_message(void) {
      if (next_empty_slot < MAX_MESSAGES) {
        erase_message(next_empty_slot);
        load_message(next_empty_slot);
        curr_msg_info.enabled = 1;
        curr_msg_info.reps = -1;
        curr_msg_info.run_interval = 35;
        curr_msg_info.fgcolor = 0xFF0000;
        put_message("YOUR MESSAGE HERE  ");
        save_message(next_empty_slot);
        update_msg_list();
        if (next_empty_slot == 255) {
          load_message(MAX_MESSAGES-1);
        } else if (next_empty_slot == 0) {
          load_message(0);
        } else {
          load_message(next_empty_slot - 1);
        }
        return true;
      } else {
        return false;
      }
    }

    // TODO: remove
    void save_sequence() {
      EEPROM.put(EEPROM_MSG_SEQ_START, msg_enabled);
    }

    // Copy message text to a character buffer
    void get_message(char * dest) {
      strncpy(dest, curr_msg_info.msg, MAX_MESSAGE_LENGTH);
    }

    // Copy message text from a character buffer
    void put_message(char * src) {
      strncpy(curr_msg_info.msg, src, MAX_MESSAGE_LENGTH);
    }
  
    Message() { 
      update_msg_list();
      load_message(msg_enabled[0]);
      init();
    }
    
    void init(void) {
      msg_pos = 0;
      char_col = 0;
      delayMillis = curr_msg_info.run_interval;
      fade = 0;
      next_message();
      state->effectInit = true;
    }

    // Generate list of enabled messages
    void update_msg_list(void) {
      memset(msg_enabled, 0, sizeof msg_enabled); // Clear list of enabled messages
      msg_count = 0; // Reset count of enabled messages to zero

      next_empty_slot = 255; // Default to no available empty slots
      
      // Count enabled messages in EEPROM
      for (uint8_t i = 0; i < MAX_MESSAGES; i++) {
        load_message(i); // Load message into RAM (no default way to read EEPROM structs in place)
        if (curr_msg_info.enabled == 1) {
          msg_enabled[msg_count++] = i | (1 << 7); // Save enabled message to list
          Serial.println(i);
        } else if (curr_msg_info.enabled > 1) { // Corrupt or new EEPROM, needs to be erased
          erase_message(i);
        }
        
        if (curr_msg_info.enabled == 255) {
          next_empty_slot = i; // Set next slot to use
          return; // Stop checking EEPROM at the first empty slot
        }
      }
    }

    // Draw bit buffer to LED matrix, advance character stream
    void render(void) {      
      CRGB pixelColor;
      delayMillis = curr_msg_info.run_interval; // update interval if it has changed since load
      for (byte x = 0; x < kMatrixWidth; x++) {
        for (byte y = 0; y < kMatrixHeight; y++) {
          if (curr_msg_info.enabled != 1) {
             pixelColor = CRGB::Black;
          } else if ((bit_buffer[(bit_buffer_idx + x) % kMatrixWidth] >> y) & 1) {
            if (curr_msg_info.style == STYLE_RAINBOW) {
              pixelColor = ColorFromPalette(RainbowColors_p, paletteCycle + y * 16, 255);
            } else {
              pixelColor = curr_msg_info.fgcolor;
            }
          } else {
            //if (style == RAINBOWBG) {
            //   pixelColor = ColorFromPalette(currentPalette, paletteCycle + y * 16, 80);
            //} else {
              pixelColor = curr_msg_info.bgcolor;  
            //}
          }
          leds[XY(x, y)] = pixelColor;
        }
      }
      if (curr_msg_info.enabled == 1) stream_chars();
      paletteCycle+=7;
    }
 };
