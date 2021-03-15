#pragma once
#include <stdint.h>

enum ReceiveState {
  RECV_IDLE,
  RECV_MSG_START,
  RECV_MSG_ADDR,
  RECV_MSG_ENABLED,
  RECV_MSG_REPS,
  RECV_MSG_RUN_INTERVAL,
  RECV_MSG_STYLE,
  RECV_MSG_FGCOLOR,
  RECV_MSG_BGCOLOR,
  RECV_MSG_TXT,
  RECV_MSG_DONE
};

enum data_char_result {
  INVALID,
  VALID,
  NEXT_FIELD,
  END_MSG
};

enum command_types {
    ERASE_LAST_MSG = 86,
    SAVE_MSG = 99,
    ADD_NEW_MSG = 42,
    GET_FW_VERSION = 77,
};

void process_serial(void);
