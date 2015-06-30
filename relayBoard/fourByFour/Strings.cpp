#include "Strings.h"

#include <avr/pgmspace.h>

const char _command_id[] PROGMEM = "id";
const PStr command_id(_command_id);

const char _command_enum[] PROGMEM = "enum";
const PStr command_enum(_command_enum);

const char _command_ser[] PROGMEM = "ser";
const PStr command_ser(_command_ser);

const char _command_set[] PROGMEM = "set";
const PStr command_set(_command_set);

const char _error_pollables[] PROGMEM = "ERROR: Too many pollables for the given array size";
const PStr error_pollables(_error_pollables);

const char _string_prompt[] PROGMEM = "$ ";
const PStr string_prompt(_string_prompt);

const char _product_details[] PROGMEM = "4x4 relay 1.0.0.0 ";
const PStr product_details(_product_details);

const char _error_serUsage[] PROGMEM = "Usage: ser <serialNumber> - where serialNumber is 16 hex chars";
const PStr error_serUsage(_error_serUsage);

PStr::PStr(const char * buffer) {
  mBuffer = buffer;
}

char PStr::operator[](uint8_t i) const {
  return pgm_read_byte_near(mBuffer + i);
}

char PStr::operator[](uint8_t i) {
  return pgm_read_byte_near(mBuffer + i);
}

char PStr::charAt(uint8_t i) const {
  return pgm_read_byte_near(mBuffer + i);
}

bool PStr::equals(char * buffer, uint8_t length) const {
  for (uint8_t i = 0; ; ++i) {
    char inConst = charAt(i);
    // Check end of toCompare
    if (inConst == 0) {
      if (i == length) {
        return true;
      } else {
        return 0;
      }
    }

    // Check end of buffer
    if (i == length) {
      return 0;
    }

    if (buffer[i] != inConst) {
      return 0;
    }
  }
}

bool PStr::begins(char * buffer, uint8_t length) const {
  for (uint8_t i=0; ; ++i) {
    char inConst = charAt(i);
    // Check end of this
    if (inConst == 0) {
      return true;
    }

    if (i == length) {
      return false;
    }

    if (buffer[i] != inConst) {
      return false;
    }
  }
}

void PStr::print() const {
  for (uint8_t i = 0; ; ++i) {
    char next = charAt(i);
    if (next == 0) {
      return;
    }
    Serial.print(next);
  }
}

void PStr::println() const {
  print();
  Serial.println();
}

