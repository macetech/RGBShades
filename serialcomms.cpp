#include "serialcomms.h"
#include "FastLED.h"
#include "messages.h"

#define FW_VERSION_STRING "{fw:1.0.0}"


extern Message text1;

enum data_char_result validate_data_char(char temp_char) {
  if ((temp_char >= 32 && temp_char <= 122)) {
    return VALID;
  } else if (temp_char == '|') {
    return NEXT_FIELD;
  } else if (temp_char == '}') {
    return END_MSG;
  } else {
    return INVALID;
  }
}

static void increment_data_field(enum ReceiveState * state) {
  if (*state == RECV_MSG_ADDR) *state = RECV_MSG_ENABLED;
  else if (*state == RECV_MSG_ENABLED) *state = RECV_MSG_REPS;
  else if (*state == RECV_MSG_REPS) *state = RECV_MSG_RUN_INTERVAL;
  else if (*state == RECV_MSG_RUN_INTERVAL) *state = RECV_MSG_STYLE;
  else if (*state == RECV_MSG_STYLE) *state = RECV_MSG_FGCOLOR;
  else if (*state == RECV_MSG_FGCOLOR) *state = RECV_MSG_BGCOLOR;
  else if (*state == RECV_MSG_BGCOLOR) *state = RECV_MSG_TXT;
  else if (*state == RECV_MSG_TXT) *state = RECV_IDLE;
}

static uint32_t CRGB_to_uint32(CRGB crgb) {
  uint32_t ret = 0;
  ret |= (uint32_t)crgb.r << 16;
  ret |= (uint32_t)crgb.g << 8;
  ret |= (uint32_t)crgb.b;
  return ret;
}

void transmit_message(Message * msg) {
  // Instead of constructing a big snprintf buffer, just save the RAM and do it the naive way with a bunch of transmits
  char fieldbuf[16] = {0};

  // Start packet and send current message address
  sprintf(fieldbuf, "{%d|", msg->curr_msg);
  Serial.print(fieldbuf);

  // Send message enabled
  sprintf(fieldbuf, "%d|", msg->curr_msg_info.enabled);
  Serial.print(fieldbuf);

  // Send message reps
  sprintf(fieldbuf, "%d|", msg->curr_msg_info.reps);
  Serial.print(fieldbuf);

  // Send message run_interval
  sprintf(fieldbuf, "%d|", msg->curr_msg_info.run_interval);
  Serial.print(fieldbuf);

  // Send message style
  sprintf(fieldbuf, "%d|", msg->curr_msg_info.style);
  Serial.print(fieldbuf);

  // Send message fgcolor
  sprintf(fieldbuf, "%lX|", CRGB_to_uint32(msg->curr_msg_info.fgcolor));
  Serial.print(fieldbuf);

  // Send message bgcolor
  sprintf(fieldbuf, "%lX|", CRGB_to_uint32(msg->curr_msg_info.bgcolor));
  Serial.print(fieldbuf);

  // Send message text
  Serial.print(msg->curr_msg_info.msg);
  Serial.println('}');
  
}

void transmit_fw_version(void) {
  Serial.println(FW_VERSION_STRING);
}

static void store_data_field(enum ReceiveState * state, char * buf, Message * msg) {
  int32_t temp_val = 0;
  
  if (*state == RECV_MSG_ADDR) {
    temp_val = atoi(buf);
    if (temp_val < Message::MAX_MESSAGES && msg->curr_msg != temp_val) {
      msg->save_message(msg->curr_msg);
      msg->load_message(temp_val);
    }
  } else if (*state == RECV_MSG_ENABLED) {
    temp_val = atoi(buf);
    if (temp_val == ERASE_LAST_MSG) { // Send address 86 to erase current pattern
      msg->erase_last_message();
    } else if (temp_val == SAVE_MSG) { // Send address 99 to save current pattern
      msg->save_message(msg->curr_msg);
    } else if (temp_val == ADD_NEW_MSG) { // Send address 99 to save current pattern
      msg->add_new_message();
    } else if (temp_val == GET_FW_VERSION) { // Send address 99 to save current pattern
      transmit_fw_version();
    } else {
      if (msg->curr_msg_info.enabled != temp_val) { // Check string length on enable edge
        if (strlen(msg->curr_msg_info.msg) > 0) {
          msg->curr_msg_info.enabled = temp_val;    
        } else {
          msg->curr_msg_info.enabled = 0;
        }
      }
    }
  } else if (*state == RECV_MSG_REPS) {
    msg->curr_msg_info.reps = atoi(buf);
  } else if (*state == RECV_MSG_RUN_INTERVAL) {
    msg->curr_msg_info.run_interval = atoi(buf);
  } else if (*state == RECV_MSG_STYLE) {
    msg->curr_msg_info.style = atoi(buf);
  } else if (*state == RECV_MSG_FGCOLOR) {
    temp_val = strtoul(buf, NULL, 16);
    msg->curr_msg_info.fgcolor = temp_val;
  } else if (*state == RECV_MSG_BGCOLOR) {
    temp_val = strtoul(buf, NULL, 16);
    msg->curr_msg_info.bgcolor = temp_val;
  } else if (*state == RECV_MSG_TXT) {
    if (strlen(buf) > 0) msg->put_message(buf);
  }
}

void process_serial(void) {
  static char serbuf[96] = {0};
  static byte seridx = 0;

  static enum ReceiveState state = RECV_IDLE;
  
  while (Serial.available()) {
    uint8_t tempChar = Serial.read();

    // Restart that packet if we got a new bracket!
    if (tempChar == '{') state = RECV_MSG_START;
    switch (state) {

      case RECV_MSG_START:
      state = RECV_MSG_ADDR;        // next byte will be start of data
      seridx = 0;                   // reset packet buffer
      memset(serbuf, 0, sizeof serbuf); // clear packet buffer
      break;

      // Receive bytes for current data field
      case RECV_MSG_ADDR:
      case RECV_MSG_ENABLED:
      case RECV_MSG_REPS:
      case RECV_MSG_RUN_INTERVAL:
      case RECV_MSG_STYLE:
      case RECV_MSG_FGCOLOR:
      case RECV_MSG_BGCOLOR:
      case RECV_MSG_TXT:
        switch (validate_data_char(tempChar)) {
          
          case VALID:
          serbuf[seridx++] = tempChar;
          break;

          case INVALID:
          state = RECV_IDLE;
          break;

          case NEXT_FIELD:
          serbuf[seridx] = 0; // add null char to end of string in buffer
          seridx = 0; // reset packet buffer
          store_data_field(&state, serbuf, &text1);
          increment_data_field(&state);
          break;

          case END_MSG:
          serbuf[seridx] = 0;
          store_data_field(&state, serbuf, &text1);
          transmit_message(&text1);
          state = RECV_MSG_DONE;
          set_serial_active(true);
          break;
          
        }
      break;

      // Stay in RECV_DONE until new message is started
      case RECV_MSG_DONE:
      break;

      default:
      state = RECV_IDLE;
      break;
      
    }
  }
}
