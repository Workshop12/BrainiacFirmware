#include "Strings.h"

#include <avr/pgmspace.h>

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
  for (uint8_t i = 0; ; ++i) {
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

